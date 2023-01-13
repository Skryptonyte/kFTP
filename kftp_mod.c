#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>  

#include <net/net_namespace.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>

#include "ftp_serv.h"
#include "ftp_utils.h"
MODULE_LICENSE("GPL");

static char *startmsg = "220 kFTP - the kernel level FTP server nobody asked for!\r\n";
static struct socket *ftpsock;

void processFTP(struct socket* clientsock,char* buf)
{

    printk(KERN_INFO "kftp: recieved FTP command: %s",buf);
    
    if (!strncmp(buf,"USER",4))
    {
        ftpUSER(clientsock);
    }
    else if (!strncmp(buf,"PASS",4))
    {
        ftpPASS(clientsock);
    }
    else
    {
        char* response = "500 unrecognized command!\r\n";
        kernel_send(clientsock,response,strlen(response));
    }
}

int recieve_loop(struct socket* clientsock)
{
    printk("kftp: beginning client loop");
    kernel_send(clientsock,startmsg,strlen(startmsg));
    char buf[256];
    while (1)
    {
        int r = kernel_recv(clientsock,buf,256);
        if (r <= 0)
        {
            printk(KERN_ERR "Message recv fail!");
            return -1;
        }
        processFTP(clientsock,buf);
    }
    return 0;
}
static int kftp_start(void)
{
    printk(KERN_INFO "kftp: starting kftp server at 2121");
    sock_create(PF_INET, SOCK_STREAM,IPPROTO_TCP,&ftpsock);


    struct sockaddr_in serv;
    serv.sin_addr.s_addr = 0;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2122);


    int bind = kernel_bind(ftpsock,(struct sockaddr*) &serv,sizeof(serv));
    if (bind < 0)
    {
        printk(KERN_ERR "kftp: failed to bind at port 2121");
        return -1;
    }
    kernel_listen(ftpsock, 10);
    static struct socket *clientsock;


    printk(KERN_INFO "Acceping connections!");

    int err = kernel_accept(ftpsock,&clientsock,0);
    if (err < 0)
        printk(KERN_ERR "kftp: failed to register new client!");
    else
    {
        printk(KERN_INFO "kftp: new client accepted: %p!",clientsock);
        recieve_loop(clientsock);
    }
    return 0;
}

static void kftp_stop(void)
{

    printk(KERN_INFO "kftp: stopping kftp server");
    sock_release(ftpsock);
}

module_init(kftp_start);
module_exit(kftp_stop);