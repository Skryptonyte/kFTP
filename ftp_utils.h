#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>

int kernel_recv(struct socket* socket, char* buf, int len);
int kernel_send(struct socket* socket, char* buf, int len);
unsigned int get_random_open_port(void);