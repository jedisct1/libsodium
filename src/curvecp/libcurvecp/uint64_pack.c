#include "uint64_pack.h"

void uint64_pack(unsigned char *y,crypto_uint64 x)
{
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
  *y++ = x; x >>= 8;
}
