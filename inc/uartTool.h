#ifndef _UARTTOOL_H
#define _UARTTOOL_H

//串口操作函数
int myserialOpen (const char *device, const int baud);
void serialSendstring (const int fd, const char *s,int len);
int serialGetstring (const int fd, char *buffer);

#define SERIAL_DEV "/dev/ttyS5"
#define BAUD 115200

#endif