#ifndef ___SOCKET_INTERFACE_H___
#define ___SOCKET_INTERFACE_H___

#include "control.h"

//将socket添加到控制链表中
struct control *add_tcpsocket_to_ctrl_list(struct control *phead);

#endif