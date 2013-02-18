#include "uint16_pack.h"

void uint16_pack(unsigned char *y,crypto_uint16 x)
{
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
}
