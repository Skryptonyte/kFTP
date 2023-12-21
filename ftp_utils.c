#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include "ftp_utils.h"
int kernel_recv(struct socket* socket, char* buf, int len)
{
    struct msghdr msg;
    struct kvec iov;

    msg.msg_name = NULL;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_namelen = 0;

    iov.iov_base = buf;
    iov.iov_len = len;


    int err = kernel_recvmsg(socket, &msg,&iov,len,len,0);
    return err;
}

int kernel_send(struct socket* socket, char* buf, int len)
{
    struct msghdr msg;
    struct kvec iov;

    msg.msg_name = NULL;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_namelen = 0;

    iov.iov_base = buf;
    iov.iov_len = len;

    int err = kernel_sendmsg(socket, &msg,&iov,len,len);
    return err;
}


unsigned int get_random_open_port(){
    unsigned long long int r;
    get_random_bytes(&r,sizeof(unsigned long long int));
    // TODO: Check if port is actually open
    return (r % 64511) + 1025;
}