#include <wiringPi.h>
#include <stdio.h>

#include "gdevice.h"

//根据key值(buffer[2])查找设备节点
struct gdevice *find_gdevice_by_key(struct gdevice *pdev, unsigned char key)
{
    struct gdevice *p = NULL;

    if (NULL == pdev)
    {
        return NULL;    //没找到时返回NULL
    }

    p = pdev;

    while (NULL != p)
    {
        if(p->key == key)
        {
            return p;   //根据对应的key值找到了对应节点并返回这个节点
        }
        p = p->next;    //遍历链表
    }

    return NULL;    //最终没找到返回NULL
}

//设置GPIO引脚状态，输入输出和高低电平
int set_gpio_gdevice_status(struct gdevice *pdev)
{
    if (NULL == pdev) 
    {  
        return -1;  //没找到节点时返回-1，说明没有设置成功
    }

    if (-1 != pdev->gpio_pin)
    {
        if (-1 != pdev->gpio_mode)
        {
            pinMode(pdev->gpio_pin, pdev->gpio_mode);   //配置引脚的输入输出模式
        }

        if (-1 != pdev->gpio_status)
        {
            digitalWrite(pdev->gpio_pin, pdev->gpio_status);    //当配置为输出模式时，引脚的高低状态 
        }
    }

    return 0;
}

//链表头插法 
struct gdevice *add_device_to_gdevice_list(struct gdevice *pdevhead, struct gdevice *device)
{
    struct gdevice *pgdevice;
    if(NULL == pdevhead)
    {
        pgdevice = device;

        return pgdevice;
    }else
    {
        device->next = pdevhead;
        pdevhead = device;

        return pdevhead;
    }
}