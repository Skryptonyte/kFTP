

struct ftp_entry {
    struct dir_context dir_ctx;
    struct socket* socket;
};

int ftp_readdir(const char* path, struct socket* dataconnclient);
int ftp_filldir(struct dir_context* ctx, const char* name, int name_len, loff_t offset, uint64_t ino, unsigned int d_type);