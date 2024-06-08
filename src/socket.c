#include "socket.h"

int socket_init(const char *ipaddr,const char *port){

    int s_fd = -1;
    int ret = -1;
    struct sockaddr_in s_addr;

    memset(&s_addr,0,sizeof(struct sockaddr_in));

    s_fd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == s_fd){
        perror("socket");
        return -1;
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(port));
    inet_aton(ipaddr,&s_addr.sin_addr);

    ret = bind(s_fd,(struct sockaddr *)&s_addr,sizeof(struct sockaddr_in));
    if(-1 == ret){
        perror("bind");
        return -1;
    }

    ret = listen(s_fd,1);
    if(-1 == ret){
        perror("listen");
        return -1;
    }

    return s_fd;
} 