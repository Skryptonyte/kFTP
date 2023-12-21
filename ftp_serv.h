

struct client_context {
    int passive;
    struct socket *controlconnclient;
    struct socket *dataconnclient;
    struct socket *dataconnserver;
};

void ftpUSER(struct client_context* ctx, struct socket* clientsock);
void ftpPASS(struct client_context* ctx, struct socket* clientsock);
void ftpSYST(struct client_context* ctx, struct socket* clientsock);
void ftpPASV(struct client_context* ctx, struct socket* clientsock);
void ftpQUIT(struct client_context* ctx, struct socket* clientsock);
void ftpTYPE(struct client_context* ctx, struct socket* clientsock);
void ftpRETR(struct client_context* ctx, struct socket* clientsock);

int create_passive_data_connection(struct client_context* ctx, int data_port);