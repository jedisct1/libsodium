#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef _WIN32
# include <windows.h>
#endif

/* it's really stupid that there isn't a syscall for this */
/* -> Y U NO USE OPENBSD? */

static int fd = -1;

void randombytes(unsigned char *x,unsigned long long xlen)
{
  int i;

  if (fd == -1) {
    for (;;) {
      fd = open("/dev/urandom",O_RDONLY);
      if (fd != -1) break;
#ifdef _WIN32
      Sleep(1000);
#else
      sleep(1);
#endif
    }
  }

  while (xlen > 0) {
    if (xlen < 1048576) i = xlen; else i = 1048576;

    i = read(fd,x,i);
    if (i < 1) {
#ifdef _WIN32
      Sleep(1000);
#else
      sleep(1);
#endif
      continue;
    }

    x += i;
    xlen -= i;
  }
}
