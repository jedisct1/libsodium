#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "open.h"
#include "savesync.h"
#include "writeall.h"

static int writesync(int fd,const void *x,long long xlen)
{
  if (writeall(fd,x,xlen) == -1) return -1;
  return fsync(fd);
}

int savesync(const char *fn,const void *x,long long xlen)
{
  int fd;
  int r;
  fd = open_write(fn);
  if (fd == -1) return -1;
  r = writesync(fd,x,xlen);
  close(fd);
  return r;
}
