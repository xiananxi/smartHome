#include <stdio.h>
#include "msg_queue.h"

#define QUEQUE_NAME "/mq_queue"

//创建消息队列
mqd_t msg_queue_create(void)
{
    mqd_t mqd = -1;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    mqd = mq_open(QUEQUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    printf("%s| %s |%d: mqd = %d\n",__FILE__, __func__, __LINE__, mqd);

    return mqd;
}

void msg_queue_final(mqd_t mqd)
{
    if (-1 != mqd)
        mq_close(mqd);      //关闭消息队列

    mq_unlink(QUEQUE_NAME);     //删除消息队列

    mqd = -1;
}

//发送消息，把语音监听线程得到的消息发送给消息队列
    //void *msg，用void类型，后面如果要传结构体之类的数据，方便做传参
int send_message(mqd_t mqd, void *msg, int msg_len)
{
    int byte_send = -1; 

    //函数默认要求char *的参数
    byte_send = mq_send(mqd, (char *)msg, msg_len, 0);

    //通过byte_send去确定发送的数据是否是准确的
    return byte_send;
}