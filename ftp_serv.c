
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <net/sock.h>

#include "ftp_utils.h"
#include "ftp_serv.h"
#include "ftp_list.h"
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
    ctx->authorized = 1;
}

void ftpSYST(struct client_context* ctx, struct socket* clientsock)
{
    char* response = "215 UNIX Type: L8\r\n";
    printk(KERN_INFO "kftp: Received SYST\n");
    kernel_send(clientsock,response,strlen(response));
}


void ftpPASV(struct client_context* ctx, struct socket* clientsock)
{
    if (!check_client_auth(ctx)){
        return;
    }
    printk(KERN_INFO "kftp: Received PASV\n");
    create_passive_data_connection(ctx, get_random_open_port(), false);
}

void ftpEPSV(struct client_context* ctx, struct socket* clientsock)
{
    if (!check_client_auth(ctx)){
        return;
    }
    printk(KERN_INFO "kftp: Received EPSV\n");
    create_passive_data_connection(ctx, get_random_open_port(), true);
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


void ftpRETR(struct client_context* ctx, struct socket* clientsock, const char* path)
{
    char* response_open = "150 Opening connection\r\n";
    char* response_file_open_fail = "550 Failed to open file\r\n";
    char* response_file_transfer_success = "226 File download successful\r\n";
    char message[256];
    struct file* file = kernel_retr_file(path);

    if (!check_client_auth(ctx)){
        clean_up_dtp(ctx);
        return;
    }
    if (!check_dtp(ctx))
    {
        clean_up_dtp(ctx);
        return;
    }
    printk(KERN_INFO "kftp: Received RETR\n");
    printk(KERN_INFO "Transferring file: %s\n", path);


    int err = kernel_accept(ctx->dataconnserver,&ctx->dataconnclient,0);
    if (err < 0)
    {
        printk(KERN_ERR "kftp: failed to register new client!\n");
        clean_up_dtp(ctx);
        return;
    }
    else
    {
        printk(KERN_INFO "kftp: new client accepted: %p!\n",clientsock);
    }
    if (!file)
    {
        printk(KERN_INFO "kftp: WARN: failed to open file: %s!\n",path);
        kernel_send(ctx->controlconnclient,response_file_open_fail,strlen(response_file_open_fail));
        clean_up_dtp(ctx);
        return;
    }
    kernel_send(ctx->controlconnclient,response_open,strlen(response_open));

    loff_t pos = 0;
    while (1) {
        memset(message,0,256);
        int ret = kernel_retr_read_file(file, message, 256, &pos);
        printk(KERN_INFO "Fetching %d bytes at offset %d\n",ret,pos);
        if (ret <= 0)
            break;
        kernel_send(ctx->dataconnclient, message,256);
    }
    
    kernel_send(ctx->controlconnclient,response_file_transfer_success,strlen(response_file_transfer_success));

    clean_up_dtp(ctx);
}

void ftpLIST(struct client_context* ctx, struct socket* clientsock, const char* path)
{
    char* response_open = "150 Opening connection\r\n";
    char* response_file_open_fail = "550 Failed to open file\r\n";
    char* response_file_transfer_success = "226 directory list complete\r\n";
    char message[256];

    if (!check_client_auth(ctx)){
        clean_up_dtp(ctx);
        return;
    }
    if (!check_dtp(ctx))
    {
        clean_up_dtp(ctx);
        return;
    }
    printk(KERN_INFO "kftp: Received LIST\n");

    int err = kernel_accept(ctx->dataconnserver,&ctx->dataconnclient,0);
    if (err < 0)
    {
        printk(KERN_ERR "kftp: failed to register new client!\n");
        clean_up_dtp(ctx);
        return;
    }
    else
    {
        printk(KERN_INFO "kftp: new client accepted: %p!\n",clientsock);
    }

    kernel_send(ctx->controlconnclient,response_open,strlen(response_open));

    memset(message, 0, 256);
    int readdir_res = ftp_readdir(path, ctx->dataconnclient);

    kernel_send(ctx->dataconnclient, message,256);
    
    kernel_send(ctx->controlconnclient,response_file_transfer_success,strlen(response_file_transfer_success));

    clean_up_dtp(ctx);
}

int create_passive_data_connection(struct client_context* ctx, int data_port, bool extended){
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

    if (extended)
    {
        sprintf(response,"229 Entering Extended Passive Mode (|||%d|)\r\n",data_port);
    }
    else{
        sprintf(response,"227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n",(data_port>>8)&0xff,data_port&0xff);
    }
    printk(KERN_INFO "kftp: PASV response - %s\n",response);
    kernel_send(ctx->controlconnclient,response,strlen(response));
    
    return 0;
}


int check_client_auth(struct client_context* ctx)
{
    if (ctx->authorized == 1)
        return 1;
    char* response = "530 Not authorized\r\n";
    kernel_send(ctx->controlconnclient,response,strlen(response));
    return 0;
}

int check_dtp(struct client_context* ctx)
{
    if (ctx->dataconnserver)
        return 1;
    char* response = "425 Server DTP not open (Use PASV)\r\n";
    kernel_send(ctx->controlconnclient,response,strlen(response));
    return 0;
}

void clean_up_dtp(struct client_context* ctx)
{
    if (ctx->dataconnclient){
        sock_release(ctx->dataconnclient);
        ctx->dataconnclient = NULL;
    }
    if (ctx->dataconnserver){
        sock_release(ctx->dataconnserver);
        ctx->dataconnserver = NULL;
    }
}
