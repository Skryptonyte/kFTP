#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/fs.h>

int kernel_recv(struct socket* socket, char* buf, int len);
int kernel_send(struct socket* socket, char* buf, int len);
unsigned int get_random_open_port(void);
struct file* kernel_retr_file(char* path);
ssize_t kernel_retr_read_file(struct file* f, char* buf, int len, loff_t* pos);
