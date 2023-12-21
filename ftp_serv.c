
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <net/sock.h>

#include "ftp_utils.h"
#include "ftp_serv.h"

void ftpUSER(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "331 placeholder\r\n";
    printk(KERN_INFO "kftp: Received USER\n");
    kernel_send(clientsock,response,strlen(response));
}

void ftpPASS(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "230 placeholder\r\n";
    printk(KERN_INFO "kftp: Received PASS\n");
    kernel_send(clientsock,response,strlen(response));
}

void ftpSYST(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "215 UNIX Type: L8\r\n";
    printk(KERN_INFO "kftp: Received SYST\n");
    kernel_send(clientsock,response,strlen(response));
}



void ftpPASV(struct client_context* ctx, struct socket* clientsock)
{
    printk(KERN_INFO "kftp: Received PASV\n");
    create_passive_data_connection(ctx, get_random_open_port());
}

void ftpQUIT(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "221 Quit Acknowledged\r\n";
    printk(KERN_INFO "kftp: Received QUIT\n");
    kernel_send(clientsock,response,strlen(response));
}

void ftpTYPE(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "200 MODE SET TO I\r\n";
    printk(KERN_INFO "kftp: Received TYPE\n");
    kernel_send(clientsock,response,strlen(response));
}


void ftpRETR(struct client_context* ctx, struct socket* clientsock)
{
    char* response_open = "150 Opening connection\r\n";
    char* response_file_transfer_success = "226 File download successful\r\n";
    char message[256];
    printk(KERN_INFO "kftp: Received RETR\n");

    int err = kernel_accept(ctx->dataconnserver,&ctx->dataconnclient,0);
    if (err < 0)
    {
        printk(KERN_ERR "kftp: failed to register new client!\n");
        return;
    }
    else
    {
        printk(KERN_INFO "kftp: new client accepted: %p!",clientsock);
    }

    kernel_send(clientsock,response_open,strlen(response_open));

    strcpy(message, "Test Message - from kFTP, the world's first kernel-level FTP server\n");
    kernel_send(ctx->dataconnclient, message,strlen(message));
    
    kernel_send(clientsock,response_file_transfer_success,strlen(response_file_transfer_success));

    sock_release(ctx->dataconnclient);
    sock_release(ctx->dataconnserver);
    ctx->dataconnclient = NULL;
    ctx->dataconnserver = NULL;
}

int create_passive_data_connection(struct client_context* ctx, int data_port){
    struct sockaddr_in serv;
    char response[256];

    sock_create(PF_INET, SOCK_STREAM,IPPROTO_TCP,&ctx->dataconnserver);

    if (!ctx->dataconnserver)
    {
        printk(KERN_ERR "kftp: socket creation failed!\n");
        return -1;
    }
    printk(KERN_INFO "kftp: data socket initialized!\n");

    serv.sin_addr.s_addr = 0;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(data_port);

    int bind = kernel_bind(ctx->dataconnserver,(struct sockaddr*) &serv,sizeof(serv));
    if (bind < 0)
    {
        printk(KERN_ERR "kftp: failed to set up data connection\n");
        return -1;
    }
    printk(KERN_INFO "kftp: data connection binding successful\n");
    
    kernel_listen(ctx->dataconnserver, 5);

    sprintf(response,"227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n",(data_port>>8)&0xff,data_port&0xff);
    printk(KERN_INFO "kftp: PASV response - %s\n",response);
    kernel_send(ctx->controlconnclient,response,strlen(response));
    

    
    return 0;
}
