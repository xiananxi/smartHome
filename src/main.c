#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "voice_interface.h"
#include "socket_interface.h"
#include "msg_queue.h"
#include "control.h"
#include "global.h"

int main()
{
    pthread_t thread_id;
    //struct control *control_phead = NULL;   //创建结构体的指针
    struct control *pointer = NULL;

    ctrl_info_t *ctrl_info = NULL;
    ctrl_info = (ctrl_info_t *)malloc(sizeof(ctrl_info_t));
    ctrl_info->ctrl_phead = NULL;
    ctrl_info->mqd = -1;

    int node_num = 0;

    ctrl_info->mqd = msg_queue_create(); // 创建消息队列
    if(ctrl_info->mqd == -1)
    {
        printf("msg_queue_create failed\n");
        return -1;
    }
    
    ctrl_info->ctrl_phead = add_voice_to_ctrl_list(ctrl_info->ctrl_phead);   //添加语音监听线程
    ctrl_info->ctrl_phead = add_socket_to_ctrl_list(ctrl_info->ctrl_phead);  //添加socket监听线程
    ctrl_info->ctrl_phead = add_smoke_to_ctrl_list(ctrl_info->ctrl_phead);   //添加烟雾报警线程

    pointer = ctrl_info->ctrl_phead;
    while(pointer != NULL)
    {
        if(pointer->init != NULL)
        {
            pointer->init();
        }
        pointer = pointer->next;
        node_num++;     //循环遍历得到链表节点的个数
    }

    pthread_t *tid = malloc(sizeof(ctrl_info_t) * node_num);    //给链表中所有的线程开辟空间

    //pointer重新指向头节点，方便从头开始循环遍历节点
    pointer = ctrl_info->ctrl_phead;
    for(int i = 0; i < node_num; i++)
    {
        if(pointer->get != NULL)
        { 
            pthread_create(&tid[i], NULL, (void *)pointer->get,NULL);           
        }
    }

    for(int i = 0; i < node_num; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for(int i = 0; i < node_num; i++)
    {
        if(pointer->final != NULL)
        {
            pointer->final();   //线程控制的结束
        }
        pointer = pointer->next;
    }

    msg_queue_final(ctrl_info->mqd);    //消息队列的关闭

    return 0;
}