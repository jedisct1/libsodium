#include "uint64_unpack.h"

crypto_uint64 uint64_unpack(const unsigned char *x)
{
  crypto_uint64 result;
  result = x[7];
  result <<= 8; result |= x[6];
  result <<= 8; result |= x[5];
  result <<= 8; result |= x[4];
  result <<= 8; result |= x[3];
  result <<= 8; result |= x[2];
  result <<= 8; result |= x[1];
  result <<= 8; result |= x[0];
  return result;
}
