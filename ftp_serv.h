

struct client_context {
    int passive;
    int authorized;
    struct socket *controlconnclient;
    struct socket *dataconnclient;
    struct socket *dataconnserver;
};

void ftpUSER(struct client_context* ctx, struct socket* clientsock);
void ftpPASS(struct client_context* ctx, struct socket* clientsock);
void ftpSYST(struct client_context* ctx, struct socket* clientsock);
void ftpPASV(struct client_context* ctx, struct socket* clientsock);
void ftpEPSV(struct client_context* ctx, struct socket* clientsock);
void ftpQUIT(struct client_context* ctx, struct socket* clientsock);
void ftpTYPE(struct client_context* ctx, struct socket* clientsock);
void ftpRETR(struct client_context* ctx, struct socket* clientsock, const char* path);
void ftpLIST(struct client_context* ctx, struct socket* clientsock, const char* path);

int create_passive_data_connection(struct client_context* ctx, int data_port, bool extended);
int check_client_auth(struct client_context* ctx);
int check_dtp(struct client_context* ctx);
void clean_up_dtp(struct client_context* ctx);