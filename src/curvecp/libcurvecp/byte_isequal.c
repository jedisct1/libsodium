#include "byte.h"

int byte_isequal(const void *yv,long long ylen,const void *xv)
{
  const unsigned char *y = yv;
  const unsigned char *x = xv;
  unsigned char diff = 0;
  while (ylen > 0) { diff |= (*y++ ^ *x++); --ylen; }
  return (256 - (unsigned int) diff) >> 8;
}
