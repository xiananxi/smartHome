#include <pthread.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

#include "wiringPi.h"
#include "control.h"
#include "receive_interface.h"
#include "msg_queue.h"
#include "global.h"
#include "face.h"
#include "myoled.h"
#include "ini.h"
#include "gdevice.h"

typedef struct {
    int msg_len;
    unsigned char *buffer;
    ctrl_info_t *ctrl_info;
}recv_msg_t;

static int oled_fd = -1;
static struct gdevice *pdevhead = NULL;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0;

static int handler_gdevice(void* user, const char* section, const char* name,const char* value)
{
    struct gdevice *pdev = NULL;

    if (NULL == pdevhead)
    {
        pdevhead = (struct gdevice *)malloc(sizeof(struct gdevice));
        pdevhead->next = NULL;
        memset(pdevhead, 0, sizeof(struct gdevice));
        strcpy(pdevhead->dev_name, section);
    }
    else if (0 != strcmp(section, pdevhead->dev_name))
    {
        pdev = (struct gdevice *)malloc(sizeof(struct gdevice));
        memset(pdev, 0, sizeof(struct gdevice));
        strcpy(pdev->dev_name, section);
        pdev->next = pdevhead;
        pdevhead = pdev;
    }

    if (NULL != pdevhead)
    {
        if(MATCH(pdevhead->dev_name, "key"))
        {
            sscanf(value, "%x", &pdevhead->key);
            printf("%d|pdevhead->key=%x\n",__LINE__, pdevhead->key);
        }
        else if(MATCH(pdevhead->dev_name, "gpio_pin"))
        {
            pdevhead->gpio_pin = atoi(value);
        }
        else if(MATCH(pdevhead->dev_name, "gpio_mode"))
        {
            if(strcmp(value, "OUTPUT") == 0)
            {
                pdevhead->gpio_mode = OUTPUT; //OUTPUT
            }
            else if (strcmp(value, "INPUT") == 0)
            {
                pdevhead->gpio_mode = INPUT;
            }
        }
        else if(MATCH(pdevhead->dev_name, "gpio_status"))
        {
            if(strcmp(value, "LOW") == 0)
            {
                pdevhead->gpio_mode = LOW; //OUTPUT
            }
            else if (strcmp(value, "HIGH") == 0)
            {
                pdevhead->gpio_mode = HIGH;
            }
        }
        else if(MATCH(pdevhead->dev_name, "check_face_status"))
        {
            pdevhead->check_face_status = atoi(value);
        }
        else if(MATCH(pdevhead->dev_name, "voice_set_status"))
        {
            pdevhead->voice_set_status = atoi(value);
        }
    }
    return 1;
}

static int receive_init(void)
{
    if (ini_parse("/etc/gdevice.ini", handler_gdevice, NULL) < 0)
    {
        printf("Can't load 'gdevice.ini'\n");
        return 1;
    }
    oled_fd = myoled_init();
    face_init();

    return oled_fd;
}

static void receive_final(void)
{
    face_final();
    if(oled_fd != -1)
    {
        close(oled_fd);
        oled_fd = -1;
    }
}

static void *handle_device(void *arg)
{
    recv_msg_t *recv_msg = NULL;
    struct gdevice *cur_gdev = NULL;
    char success_or_failed[20] = "success";
    int ret = -1;
    pthread_t tid = -1;
    int smoke_status = 0;
    double face_result = 0.0;

    pthread_detach(pthread_self());

    //do something
    if (NULL != arg)
    {
        recv_msg = (recv_msg_t *)arg;   //设备链表传进来
        printf("recv_msg->msg_len = %d\n", recv_msg->msg_len);
        printf("%s|%s|%d:hanle 0x%x, 0x%x,0x%x, 0x%x, 0x%x,0x%x\n", __FILE__,__func__, __LINE__, recv_msg->buffer[0], recv_msg->buffer[1], recv_msg->buffer[2], recv_msg->buffer[3], recv_msg->buffer[4],recv_msg->buffer[5]);
    }

    if (NULL != recv_msg && NULL != recv_msg->buffer)
    {
        cur_gdev = find_device_by_key(pdevhead, recv_msg->buffer[2]);   //找到当前的设备
    }

    if (NULL != cur_gdev)
    {
        cur_gdev->gpio_status = recv_msg->buffer[3] == 0 ? LOW : HIGH; //继电器中0打开，低电平有效
        //special for lock
        printf("%s|%s|%d:cur_gdev->check_face_status=%d\n", __FILE__, __func__,__LINE__, cur_gdev->check_face_status);
        
        if (1 == cur_gdev->check_face_status)
        {
            face_result = face_category();
            printf("%s|%s|%d:face_result=%f\n", __FILE__, __func__, __LINE__,face_result);
            if (face_result > 0.6)
            {
                ret = set_gpio_gdevice_status(cur_gdev);    //传设备控制的结构体，设置引脚的状态
                recv_msg->buffer[2] = 0x47;
            }
            else
            {
                recv_msg->buffer[2] = 0x46;
                ret = -1;
            }
        }
        else if (0 == cur_gdev->check_face_status)
        {
            ret = set_gpio_gdevice_status(cur_gdev);    
        }

        if(1 == cur_gdev->voice_set_status)
        {
            if (NULL != recv_msg && NULL != recv_msg->ctrl_info && NULL !=recv_msg->ctrl_info->ctrl_phead)
            {
                struct control *pcontrol = recv_msg->ctrl_info->ctrl_phead;
                while (NULL != pcontrol)
                {
                    if (strstr(pcontrol->control_name, "voice"))
                    {
                        if (0x45 == recv_msg->buffer[2] && 0 == recv_msg->buffer[3])
                        {
                            smoke_status = 1;
                        }
                        pthread_create(&tid, NULL, pcontrol->set, (void *)recv_msg->buffer);
                        break;
                    }
                    pcontrol = pcontrol->next;
                }
            }
        }
        if (-1 == ret)
        {
            memset(success_or_failed, '\0', sizeof(success_or_failed));
            strncpy(success_or_failed, "failed", 6);
        }
        //oled屏显示
        char oled_msg[512];
        memset(oled_msg, 0, sizeof(oled_msg));
        char *change_status = cur_gdev->gpio_status == LOW ? "Open" : "Close";
        sprintf(oled_msg, "%s %s %s!\n", change_status, cur_gdev->dev_name,success_or_failed);
        //special for smoke
        if(smoke_status == 1)
        {
            memset(oled_msg, 0, sizeof(oled_msg));
            strcpy(oled_msg, "A risk of fire!\n");
        }
        printf("oled_msg=%s\n", oled_msg);
        oled_show(oled_msg);
        //special for lock, close lock
        if (1 == cur_gdev->check_face_status && 0 == ret && face_result > 0.6)
        {
            sleep(5);
            cur_gdev->gpio_status = HIGH;
            set_gpio_gdevice_status(cur_gdev);
        }
    }
    pthread_exit(0);
}

static void* receive_get(void *arg)
{
    recv_msg_t *recv_msg = NULL;
    ssize_t read_len = -1;
    pthread_t tid = -1;
    char *buffer = NULL;
    struct mq_attr attr;
    if (NULL != arg)
    {
        recv_msg = (recv_msg_t *)malloc(sizeof(recv_msg_t));
        recv_msg->ctrl_info = (ctrl_info_t *)arg; //获取到mqd 和phead (structcontrol 链表的头结点)
        recv_msg->msg_len = -1;
        recv_msg->buffer = NULL;
    }
    else
    {
        pthread_exit(0);
    }

    if(mq_getattr(recv_msg->ctrl_info->mqd, &attr) == -1)
    {
        pthread_exit(0);
    }

    recv_msg->buffer = (unsigned char *)malloc(attr.mq_msgsize);
    buffer = (unsigned char *)malloc(attr.mq_msgsize);
    memset(recv_msg->buffer, 0, attr.mq_msgsize);
    memset(buffer, 0, attr.mq_msgsize);
    pthread_detach(pthread_self());
    struct timespec timeout = { .tv_sec = 5, .tv_nsec = 0 };

    while(1)
    {
        // read_len = mq_receive(recv_msg->ctrl_info->mqd, buffer,attr.mq_msgsize, NULL);
        read_len = mq_timedreceive(recv_msg->ctrl_info->mqd, buffer,attr.mq_msgsize, NULL, &timeout);
        printf("%s|%s|%d:send 0x%x, 0x%x,0x%x, 0x%x, 0x%x,0x%x\n", __FILE__,__func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3],buffer[4],buffer[5]);
        printf("%s|%s|%d:read_len=%ld\n", __FILE__, __func__, __LINE__,read_len);
        
        if (-1 == read_len)
        {
            if(errno == EAGAIN)
            {
                printf("queue is empyt\n");
            }
            else if (errno == ETIMEDOUT) {
                printf("timeout\n");
                continue;
            }
            else
            {
                break;
            }
        }
        else if(buffer[0] == 0xAA && buffer[1] == 0x55
        && buffer[5] == 0xAA && buffer[4] == 0x55)
        {
            recv_msg->msg_len = read_len;
            memcpy(recv_msg->buffer, buffer, read_len);
            pthread_create(&(tid), NULL, handle_device, (void *)recv_msg);
        }
    }
    pthread_exit(0);
}

struct control receive_control = {
    .control_name = "receive",
    .init = receive_init,
    .final = receive_final,
    .get = receive_get,
    .set = NULL,
    .next = NULL
};

struct control *add_receive_to_ctrl_list(struct control *phead)
{//头插法
    return add_interface_to_ctrl_list(phead, &receive_control);
};