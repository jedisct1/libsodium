#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "e.h"
#include "socket.h"
#include "byte.h"

int socket_send(int fd,const unsigned char *x,long long xlen,const unsigned char *ip,const unsigned char *port)
{
  struct sockaddr_in sa;

  if (xlen < 0 || xlen > 1048576) { errno = EPROTO; return -1; }

  byte_zero(&sa,sizeof sa);
  sa.sin_family = AF_INET;
  byte_copy(&sa.sin_addr,4,ip);
  byte_copy(&sa.sin_port,2,port);
  return sendto(fd,x,xlen,0,(struct sockaddr *) &sa,sizeof sa);
}
