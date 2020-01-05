typedef struct client_s
{
  int descriptor;      // socket id
  char *host;          // ip address

  char read_buffer[1024];  // network input buffer
  char write_buffer[1024]; // network output buffer

  ent_t *ent;          // entity information
  int state;           // client state

  struct client_s *next;
} client_t;

#define SERVER_DEFAULT_PORT  "27910"  // used in shared/cvar.c

client_t *createClient(void);
void destroyClient(client_t **client);
client_t *acceptClient(void);
int initServer(int port);
void serverRead(client_t *client);
int checkServerConnections(void);

#define CLIENT_AUTH         1
#define CLIENT_GET_STATE    2
#define CLIENT_CONNECTED    3
