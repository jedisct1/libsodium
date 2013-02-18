#include "uint32_unpack.h"

crypto_uint32 uint32_unpack(const unsigned char *x)
{
  crypto_uint32 result;
  result = x[3];
  result <<= 8; result |= x[2];
  result <<= 8; result |= x[1];
  result <<= 8; result |= x[0];
  return result;
}
