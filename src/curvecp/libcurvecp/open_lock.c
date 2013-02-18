#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "open.h"

int open_lock(const char *fn)
{
#ifdef O_CLOEXEC
  int fd = open(fn,O_RDWR | O_CLOEXEC);
  if (fd == -1) return -1;
#else
  int fd = open(fn,O_RDWR);
  if (fd == -1) return -1;
  fcntl(fd,F_SETFD,1);
#endif
#ifdef F_LOCK
  if (lockf(fd,F_LOCK,0) == -1) { close(fd); return -1; }
#endif
  return fd;
}
