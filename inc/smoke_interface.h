#ifndef ___SMOKE_INTERFACE_H___
#define ___SMOKE_INTERFACE_H___

#include "control.h"

//将烟感设备添加到控制列表中
struct control *add_smoke_to_ctrl_list(struct control *phead);

#endif