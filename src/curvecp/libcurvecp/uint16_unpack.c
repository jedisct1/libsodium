#include "uint16_unpack.h"

crypto_uint16 uint16_unpack(const unsigned char *x)
{
  crypto_uint16 result;
  result = x[1];
  result <<= 8; result |= x[0];
  return result;
}
