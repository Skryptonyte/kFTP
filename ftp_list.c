
#include <linux/fs.h>
#include "ftp_list.h"
#include "ftp_utils.h"
int ftp_filldir(struct dir_context* ctx, const char* name, int name_len, loff_t offset, uint64_t ino, unsigned int d_type)
{
    struct ftp_entry* ftp_ctx = container_of(ctx, struct ftp_entry, dir_ctx);
    struct path* path;

    char response[512];
    memset(response,0,512);
    snprintf(response, 512, "- %s\r\n",name);
    kernel_send(ftp_ctx->socket, response,512);
    return 0;
}

int ftp_readdir(const char* path, struct socket* dataconnclient)
{
    if (!path)
        return 0;

    struct file* f = filp_open(path,O_RDONLY,0);
    
    if (IS_ERR(f) || !f)
        return -1;

    struct ftp_entry ftp_ctx;
    ftp_ctx.dir_ctx.actor = ftp_filldir;
    ftp_ctx.socket = dataconnclient;
    
    int err = iterate_dir(f, &ftp_ctx.dir_ctx);

    return err;
}
