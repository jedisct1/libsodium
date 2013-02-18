#include "byte.h"

void byte_copy(void *yv,long long ylen,const void *xv)
{
  char *y = yv;
  const char *x = xv;
  while (ylen > 0) { *y++ = *x++; --ylen; }
}
