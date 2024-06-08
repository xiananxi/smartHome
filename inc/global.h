#ifndef __GLOBAL__H
#define __GLOBAL__H

//定义全局变量，用于mqd和控制链表的传递
typedef struct 
{
    mqd_t mqd;
    struct control *ctrl_phead;     //准备添加的各个线程的链表

}ctrl_info_t;

#endif