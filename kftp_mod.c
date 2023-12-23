#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>  

#include <net/net_namespace.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <net/sock.h>

#include "ftp_serv.h"
#include "ftp_utils.h"
MODULE_LICENSE("GPL");

static char *startmsg = "220 kFTP - the kernel level FTP server nobody asked for!\r\n";
static struct socket *ftpsock;


int processFTP(struct client_context* ctx,struct socket* clientsock,char* buf)
{

    printk(KERN_INFO "kftp: recieved FTP command: %s",buf);
    
    if (!strncmp(buf,"USER",4))
    {
        ftpUSER(ctx, clientsock);
    }
    else if (!strncmp(buf,"PASS",4))
    {
        ftpPASS(ctx, clientsock);
    }
    else if (!strncmp(buf,"SYST",4))
    {
        ftpSYST(ctx, clientsock);
    }
    else if (!strncmp(buf,"PASV",4))
    {
        ftpPASV(ctx, clientsock);
    }
    else if (!strncmp(buf,"QUIT",4))
    {
        ftpQUIT(ctx, clientsock);
    }
    else if (!strncmp(buf,"TYPE",4))
    {
        ftpTYPE(ctx, clientsock);
    }
    else if (!strncmp(buf,"RETR",4))
    {
        const char* path = strrchr(buf,' ')+1;
        if (strrchr(buf,'\r'))
            *strrchr(buf,'\r') = 0;
        ftpRETR(ctx, clientsock, path);
    }
    else
    {
        printk(KERN_INFO "kftp: Unrecognized Command\n");
        char* response = "500 UNRECOGNIZED COMMAND\r\n";
        kernel_send(clientsock,response,strlen(response));
    }
    return 0;
}

int recieve_loop(struct socket* clientsock)
{
    printk(KERN_INFO "Allocating client socket object.\n");
    struct client_context *ctx = kmalloc(sizeof(struct client_context),GFP_KERNEL);
    ctx->passive = 0;
    ctx->authorized = 0;
    ctx->dataconnclient = NULL;
    ctx->dataconnserver = NULL;
    ctx->controlconnclient = clientsock;
    printk("kftp: beginning client loop\n");
    kernel_send(clientsock,startmsg,strlen(startmsg));
    char buf[256];

    while (1)
    {
        memset(buf,0,256);
        int index = 0;
        while (index < 256)
        {
            int r = kernel_recv(clientsock,buf+index,1);
            printk("%x\n",buf[index]);
            if (r <= 0)
            {
                printk(KERN_ERR "Message recv fail!\n");
                return -1;
            }
            if (buf[index] == 0xa){
                break;
            }
            index++;
        }

        if (processFTP(ctx,clientsock,buf) != 0){
            break;
        }
    }
    printk(KERN_INFO "Freeing client socket object.\n");
    kfree(ctx);
    return 0;
}
static int kftp_start(void)
{
    printk(KERN_INFO "kftp: starting kftp server at 2121\n");
    sock_create(PF_INET, SOCK_STREAM,IPPROTO_TCP,&ftpsock);


    struct sockaddr_in serv;
    serv.sin_addr.s_addr = 0;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2122);
    int* optval = 1;
    sock_set_reuseaddr(ftpsock->sk);
    sock_set_reuseport(ftpsock->sk);

    int bind = kernel_bind(ftpsock,(struct sockaddr*) &serv,sizeof(serv));
    if (bind < 0)
    {
        printk(KERN_ERR "kftp: failed to bind at port 2121\n");
        return -1;
    }
    kernel_listen(ftpsock, 10);
    struct socket *clientsock;


    printk(KERN_INFO "Acceping connections!\n");

    int err = kernel_accept(ftpsock,&clientsock,0);
    if (err < 0)
        printk(KERN_ERR "kftp: failed to register new client!\n");
    else
    {
        printk(KERN_INFO "kftp: new client accepted: %p!",clientsock);
        recieve_loop(clientsock);
    }
    return 0;
}

static void kftp_stop(void)
{

    printk(KERN_INFO "kftp: stopping kftp server\n");
    sock_release(ftpsock);
}

module_init(kftp_start);
module_exit(kftp_stop);