
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>

#include "ftp_utils.h"
#include "ftp_serv.h"

void ftpUSER(struct socket* clientsock)
{
    printk(KERN_INFO "kftp: Received USER");
    char* response = "331 placeholder\r\n";
    kernel_send(clientsock,response,strlen(response));
}

void ftpPASS(struct socket* clientsock)
{
    printk(KERN_INFO "kftp: Received PASS");
    char* response = "230 placeholder\r\n";
    kernel_send(clientsock,response,strlen(response));
}
