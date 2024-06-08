//统一的设备类设备
#ifndef __GDEVICE_H
#define __GDEVICE_H

#include "wiringPi.h"

struct gdevice
{
    char dev_name[128]; //设备名称
    int key; //key值，用于匹配控制指令的值
    int gpio_pin; //控制的gpio引脚 6789
    int gpio_mode; //输入输出模式  INPUT OUTPUT  如果不想配置给-1
    int gpio_status; //高低电平状态   LOW HIGH   如果不想配置给-1
    int check_face_status; //是否进行人脸检测状态
    int voice_set_status; //是否语音语音播报
    
    struct gdevice *next;
};

struct gdevice *add_device_to_gdevice_list(struct gdevice *phead, struct gdevice *device);
int set_gpio_gdevice_status(struct gdevice *pdev); 
struct gdevice *find_gdevice_by_key(struct gdevice *phead, int key);    //判断控制哪个设备

#endif