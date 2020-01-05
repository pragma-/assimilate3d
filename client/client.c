#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#ifdef WIN32
  #include <winsock2.h>
#endif

#include "SDL.h"
#include "SDL_opengl.h"

#include "cvar.h"
#include "texture.h"
#include "console.h"
#include "interp.h"
#include "bsp.h"
#include "ms3d.h"
#include "model.h"
#include "entity.h"
#include "network.h"
#include "server.h"
#include "commands.h"
#include "game.h"

client_t *player_client = 0,
         *client_list = 0;

extern char net_errorString[];
extern int net_client;
extern fd_set net_read_set;

void clientCreatePlayer(void)
{
  player_client = createClient();
  player_client->host = strdup("127.0.0.1"); // fixme: create NET_getHostname()

  player_client->next = client_list;
  client_list = player_client;
}

void clientConnectServer(char *address, int port)
{
  char buf[1024];
  extern cvar_t *cvar_cl_name;

  CON_printf("Connecting to %s:%d...", address, port);

  clientCreatePlayer();

  if(!NET_connectServer(&(player_client->descriptor), address, port))
  {
    CON_printf(net_errorString);
    removeClient(player_client, &client_list);
    destroyClient(&player_client);
    return;
  }

  net_client = player_client->descriptor;

  player_client->state = CLIENT_AUTH;
  
  sprintf(buf, "auth %s %s", cvar_cl_name->value, GAME_VERSION);
  NET_writeTCP(net_client, buf);

  CON_printf("Connected to %s:%d (client id: %d)", address, port, net_client);
}

void clientDisconnect(client_t *client)
{
  NET_disconnectClient(client->descriptor);
  net_client = -1;

  destroyClient(&client);
}

void clientProcess(client_t *client)
{
  char command[1024], arguments[1024], buf[1024];
  BSPLevel_t *bsplevel;
  extern BSPLevel_t *gbsp_map;
  extern cvar_t *cvar_cl_name;

  I_halfChop(client->read_buffer, command, arguments);

  if(!strcmp(command, "echo"))
  {
    CON_printf(arguments);
  }
  else if(!strcmp(command, "auth"))
  {
    CON_printf("AUTH: [%s]", arguments);
    if(strcmp(arguments, "ok"))
    {
      CON_printf("CLIENT: AUTH failed");
    }
    else
    {
      client->state = CLIENT_GET_STATE;
      client->ent = E_createEntity(cvar_cl_name->value);
      if(!client->ent)
      {
        CON_printf("CLIENT: failed to create client entity");
        clientDisconnect(client);
      }
    }
  }
  else if(!strcmp(command, "map"))
  {
    if(!arguments)
    {
      CON_printf("CLIENT: MAP command without mapname");
      return;
    }

    if(bsplevel = BSP_loadLevel(arguments))
    {
      BSP_destroyLevel(gbsp_map);
      gbsp_map = bsplevel;

      CON_printf("Server map %s loaded", arguments);
    }
  }
}

void clientRead(client_t *client)
{
  int size;

  if(!(size = NET_read(client->descriptor, client->read_buffer, sizeof(client->read_buffer))))
  {
    CON_printf("CLIENT: Server disconnected"); 
    removeClient(player_client, &client_list);
    destroyClient(&player_client); // NET_disconnectClient occurs here
    net_client = -1;
  }
  else
  {
    CON_printf("CLIENT: Read from server: %s", client->read_buffer);
    clientProcess(client);
  }
}

void checkClientConnections(void)
{
  if(FD_ISSET(net_client, &net_read_set))
    clientRead(player_client);

  return;
}

COMMAND(CMD_connect)
{
  char address[CON_BCS], port[CON_BCS];

  I_halfChop(arguments, address, port);

  if(!*address || !*port)
  {
    CON_printf("Usage:  connect <address> <port>");
    return;
  }

  if(net_client != -1)
    CMD_disconnect(0, 0);

  clientConnectServer(address, atoi(port));
}

COMMAND(CMD_disconnect)
{
  if(net_client == -1)
    CON_printf("Not connected!");
  else
  {
    removeClient(player_client, &client_list);
    destroyClient(&player_client);
    net_client = -1;
    CON_printf("Disconnected");
  }
}

COMMAND(CMD_say)
{
  extern int net_server;
  char buf[CON_BCS];
  int i;

  if(!*arguments)
  {
    CON_printf("Usage:  say <message>");
    return;
  }

  if(net_client == -1)
    CON_printf("Not connected");
  else
  {
    sprintf(buf, "say %s", arguments);

    i = NET_writeTCP(net_client, buf);
    CON_printf("(%d) %s", i, buf);
  }
}
