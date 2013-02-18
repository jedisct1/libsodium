#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "e.h"
#include "socket.h"
#include "byte.h"

long long socket_recv(int fd,unsigned char *x,long long xlen,unsigned char *ip,unsigned char *port)
{
  struct sockaddr_in sa;
  socklen_t salen;
  int r;

  if (xlen < 0) { errno = EPROTO; return -1; }
  if (xlen > 1048576) xlen = 1048576;

  byte_zero(&sa,sizeof sa);
  salen = sizeof sa;
  r = recvfrom(fd,x,xlen,0,(struct sockaddr *) &sa,&salen);
  byte_copy(ip,4,&sa.sin_addr);
  byte_copy(port,2,&sa.sin_port);
  return r;
}
