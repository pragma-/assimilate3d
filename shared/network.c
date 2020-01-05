#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#if defined POSIX
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/time.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <arpa/inet.h>
#elif defined WIN32
  #include <winsock2.h>
#endif

#include "util.h"

int net_server = -1,
    net_client = -1,
    maxdesc    = 0;

char net_errorString[128];

fd_set net_master_set, net_read_set;

void NET_init(void)
{
  FD_ZERO(&net_master_set);
  FD_ZERO(&net_read_set);
  maxdesc = 0;
}

int NET_bindServer(int port)
{
  struct sockaddr_in sa;
  int opt = 1;  // Solaris:  use char opt = '1';

  FD_ZERO(&net_master_set);

  net_server = socket(AF_INET, SOCK_STREAM, 0);

  if(net_server == -1)
  {
    strcpy(net_errorString, "NET_bindServer:  could not create socket");
    return false;
  }

  if(setsockopt(net_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
    strcpy(net_errorString, "NET_bindServer:  could not set SO_REUSEADDR");
    close(net_server);
    net_server = -1;
    return false;
  }

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;
  memset(&(sa.sin_zero), 0, 8);

  if(bind(net_server, (struct sockaddr *)&sa, sizeof(sa)) == -1)
  {
    strcpy(net_errorString, "NET_bindServer:  could not bind socket");
    close(net_server);
    net_server = -1;
    return false;
  }

  listen(net_server, 3);

  FD_SET(net_server, &net_master_set);
  
  if(net_server > maxdesc)
    maxdesc = net_server;

  return true;
}

int NET_acceptClient(int *descriptor, char **host)
{
  struct sockaddr_in sa;
  int size = sizeof(struct sockaddr_in);

  printf("accepting socket\n");

  if((*descriptor = accept(net_server, (struct sockaddr *)&sa, &size)) == -1)
    return false;

  *host = strdup(inet_ntoa(sa.sin_addr));
  #ifdef POSIX
  fcntl(*descriptor, F_SETFL, O_NONBLOCK);
  #endif

  if(*descriptor > maxdesc)
    maxdesc = *descriptor;

  FD_SET(*descriptor, &net_master_set);

  return true;
}

void NET_disconnectClient(int descriptor)
{
  if(descriptor > 0)
  {
    close(descriptor);
    FD_CLR(descriptor, &net_master_set);
  }
}

int NET_checkConnections(void)
{
  struct timeval null_time;

  null_time.tv_sec = 0;
  null_time.tv_usec = 0;

  net_read_set = net_master_set;

  if(select(maxdesc+1, &net_read_set, NULL, NULL, &null_time) == -1)
  {
    strcpy(net_errorString, "NET_checkConnections:  select failed");
    return false;
  }
  return true;
}

int NET_connectServer(int *descriptor, char *address, int port)
{
  struct sockaddr_in sa;
  struct hostent *hent;

  if((*descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    strcpy(net_errorString, "NET_connectServer:  could not create socket");
    return 0;
  }

  if((hent = gethostbyname(address)) == NULL)
  {
    strcpy(net_errorString, "NET_connectServer:  could not gethostbyname");
    close(*descriptor);
    *descriptor = -1;
    return 0;
  }

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr = *((struct in_addr *)hent->h_addr);
  memset(&(sa.sin_zero), 0, 8);

  if(connect(*descriptor, (struct sockaddr *)&sa, sizeof(struct sockaddr)) == -1)
  {
    strcpy(net_errorString, "NET_connectServer:  connect failed");
    close(*descriptor);
    *descriptor = -1;
    return 0;
  }

  if(*descriptor > maxdesc)
    maxdesc = *descriptor;

  FD_SET(*descriptor, &net_master_set);

  // handshake and acknowledgements here
  return 1;
}

int NET_writeTCP(int socket, char *buffer)
{
  int size = strlen(buffer) + 1;
  int ret;

  ret = send(socket, buffer, size, 0);
  return ret;
}

int NET_read(int socket, char *buffer, int size)
{
  int ret;

  ret = recv(socket, buffer, size, 0);

  #ifdef WIN32
    #define EWOULDBLOCK WSAEWOULDBLOCK
  #endif

  if((ret == -1 && errno != EWOULDBLOCK) || ret == 0)
  {
    close(socket);
    FD_CLR(socket, &net_master_set);
    return 0;
  }

  buffer[ret] = 0;
  return ret;
}
