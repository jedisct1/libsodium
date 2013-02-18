#include "uint32_pack.h"

void uint32_pack(unsigned char *y,crypto_uint32 x)
{
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
}
