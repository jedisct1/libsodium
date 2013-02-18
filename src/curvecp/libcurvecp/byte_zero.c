#include "byte.h"

void byte_zero(void *yv,long long ylen)
{
  char *y = yv;
  while (ylen > 0) { *y++ = 0; --ylen; }
}
