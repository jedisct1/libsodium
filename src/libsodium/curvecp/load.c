#include <unistd.h>
#include "open.h"
#include "e.h"
#include "load.h"

static int readall(int fd,void *x,long long xlen)
{
  long long r;
  while (xlen > 0) {
    r = xlen;
    if (r > 1048576) r = 1048576;
    r = read(fd,x,r);
    if (r == 0) errno = EPROTO;
    if (r <= 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue;
      return -1;
    }
    x += r;
    xlen -= r;
  }
  return 0;
}

int load(const char *fn,void *x,long long xlen)
{
  int fd;
  int r;
  fd = open_read(fn);
  if (fd == -1) return -1;
  r = readall(fd,x,xlen);
  close(fd);
  return r;
}
