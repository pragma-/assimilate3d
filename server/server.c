#include <stdio.h>
#include <stdlib.h>

#if defined POSIX
  #include <sys/types.h>
#elif defined WIN32
  #include <winsock2.h>
#endif

#include "SDL.h"
#include "SDL_opengl.h"

#include "cvar.h"
#include "texture.h"
#include "console.h"
#include "network.h"
#include "interp.h"
#include "commands.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "server.h"

client_t *clients = 0;  // linked list of clients

extern int net_server;
extern char net_errorString[];
extern fd_set net_read_set;

client_t *createClient(void)
{
  client_t *client;

  client = malloc(sizeof(client_t));
  memset(client, 0, sizeof(client_t));

  return client;
}

void destroyClient(client_t **client)
{
  if(*client)
  {
    if((*client)->descriptor > 0)
      NET_disconnectClient((*client)->descriptor);
    if((*client)->host)
      free((*client)->host);
    if((*client)->ent)
      E_destroyEntity(&((*client)->ent));
    free(*client);
    *client = 0;
  }
}

void removeClient(client_t *client, client_t **list)
{
  client_t *temp;

  if(client == *list)
    *list = client->next;
  else
  {
    temp = *list;

    while (temp && (temp->next != (client)))
      temp = temp->next;

    if (temp)
      temp->next = client->next;
  }
}


client_t *acceptClient(void)
{
  client_t *client;

  client = createClient();

  if(!NET_acceptClient(&client->descriptor, &client->host))
  {
    destroyClient(&client);
    return 0;
  }

  client->next = clients;
  clients = client;

  return client;
}

int initServer(int port)
{
  if(net_server != -1)
  {
    CON_printf("Server already running");
    return 0;
  }

  CON_printf("Initializing server on port %d...", port);
  if(!NET_bindServer(port))
  {
    CON_printf(net_errorString);
    return 0;
  }
  CON_printf("Server successfully initialized (descriptor: %d)", net_server);
  return 1;
}

int destroyServer(void)
{
  client_t *client, *next;

  if(net_server == -1)
  { 
    CON_printf("No server running");
    return 0;
  }

  CON_printf("Shutting down server:");

  for(client = clients; client; client = next)
  {
    next = client->next;
    CON_printf("    Removing client %d, %s", client->descriptor, client->host);
    destroyClient(&client);
  }
  clients = 0;
  NET_disconnectClient(net_server);
  net_server = -1;

  CON_printf("Server shutdown successfully");
  return 1;
}

void serverProcess(client_t *client)
{
  char command[1024], arguments[1024], buf[1024];
  client_t *cl;
  ent_t *ent;

  I_halfChop(client->read_buffer, command, arguments);

  if(!strcmp(command, "auth"))
  {
    if(!*arguments)
    {
      CON_printf("SERVER: Invalid 'auth' from %d (%s)", client->descriptor, client->host);
      NET_writeTCP(client->descriptor, "Invalid 'auth'");
      NET_disconnectClient(client->descriptor);
      removeClient(client, &clients);
      destroyClient(&client);
    }
    I_halfChop(arguments, buf, command);
    if(!(client->ent = E_createEntity(buf)))
    {
      CON_printf("SERVER: Could not create entity for client %d (%s)", client->descriptor, client->host);
      NET_writeTCP(client->descriptor, "No entity available");
      NET_disconnectClient(client->descriptor);
      removeClient(client, &clients);
      destroyClient(&client);
    }
    NET_writeTCP(client->descriptor, "auth ok");
  }
  else if(!strcmp(command, "say"))
  {
    if(!client->ent)
    {
      CON_printf("SERVER:  Encountered 'say' without valid ent struct");
      return;
    }

    sprintf(buf, "echo %s^3:^8; %s", client->ent->name, arguments); 
    for(cl = clients; cl; cl = cl->next)
      NET_writeTCP(cl->descriptor, buf);
  }
}

void serverRead(client_t *client)
{
  int size;

  if(!(size = NET_read(client->descriptor, client->read_buffer, sizeof(client->read_buffer))))
  {
    CON_printf("SERVER: Connection lost for %s ([%d] %s)", 
       client->ent ? client->ent->name : "(no-name)", client->descriptor, client->host);
    removeClient(client, &clients);
    destroyClient(&client);
  }
  else
  {
    CON_printf("SERVER: Read from %s ([%d] %s): (%d) %s", 
        client->ent ? client->ent->name : "(no-name)", client->descriptor,
        client->host, size, client->read_buffer);

    serverProcess(client);
  }
}

int checkServerConnections(void)
{
  client_t *client;

  if(FD_ISSET(net_server, &net_read_set))
  {
    CON_printf("SERVER: New incoming connection requested");
    if(client = acceptClient())
      CON_printf("SERVER: New connection accepted:  %d, %s", client->descriptor, client->host);
    else
      CON_printf("SERVER: New connection failed");
  }

  for(client = clients; client; client = client->next)
    if(FD_ISSET(client->descriptor, &net_read_set))
      serverRead(client);
  
  return 1;
}

COMMAND(CMD_sv_start)
{
  extern cvar_t *cvar_serverPort;
  initServer(atoi(cvar_serverPort->value));
}

COMMAND(CMD_sv_shutdown)
{
  destroyServer();
}
