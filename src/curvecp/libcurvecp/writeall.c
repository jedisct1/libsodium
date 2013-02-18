#include <poll.h>
#include <unistd.h>
#include "e.h"
#include "writeall.h"

int writeall(int fd,const void *x,long long xlen)
{
  long long w;
  while (xlen > 0) {
    w = xlen;
    if (w > 1048576) w = 1048576;
    w = write(fd,x,w);
    if (w < 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        struct pollfd p;
	p.fd = fd;
	p.events = POLLOUT | POLLERR;
	poll(&p,1,-1);
        continue;
      }
      return -1;
    }
    x += w;
    xlen -= w;
  }
  return 0;
}
