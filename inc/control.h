//监听模块的统一接口
#ifndef __CONTROL_H
#define __CONTROL_H

#include <stdio.h>
#include <stdlib.h>

struct control
{
    char control_name[128]; //监听模块名称
    int (*init)(void); //初始化函数
    void (*final)(void);//结束释放函数
    void *(*get)(void *arg);//监听函数，如语音监听
    void *(*set)(void *arg); //设置函数，如语音播报

    struct control *next;
};

struct control *add_device_to_ctrl_list(struct control *phead, struct control *device);

#endif  
