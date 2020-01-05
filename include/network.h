void NET_init(void);
int NET_bindServer(int port);
int NET_acceptClient(int *descriptor, char **host);
void NET_disconnectClient(int descriptor);
int NET_checkConnections(void);
int NET_connectServer(int *socket, char *address, int port);
int NET_writeTCP(int socket, char *buffer);
int NET_read(int socket, char *buffer, int size);
