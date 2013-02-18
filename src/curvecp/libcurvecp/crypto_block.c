#include "crypto_block.h"
#include "crypto_uint64.h"
#include "uint64_unpack.h"
#include "uint64_pack.h"

/*
TEA with double-size words.
XXX: Switch to crypto_block_aes256.
XXX: Build crypto_stream_aes256 on top of crypto_block_aes256.
*/

int crypto_block(
  unsigned char *out,
  const unsigned char *in,
  const unsigned char *k
)
{
  crypto_uint64 v0 = uint64_unpack(in + 0);
  crypto_uint64 v1 = uint64_unpack(in + 8);
  crypto_uint64 k0 = uint64_unpack(k + 0);
  crypto_uint64 k1 = uint64_unpack(k + 8);
  crypto_uint64 k2 = uint64_unpack(k + 16);
  crypto_uint64 k3 = uint64_unpack(k + 24);
  crypto_uint64 sum = 0;
  crypto_uint64 delta = 0x9e3779b97f4a7c15;
  int i;
  for (i = 0;i < 32;++i) {
    sum += delta;
    v0 += ((v1<<7) + k0) ^ (v1 + sum) ^ ((v1>>12) + k1);
    v1 += ((v0<<16) + k2) ^ (v0 + sum) ^ ((v0>>8) + k3);
  }
  uint64_pack(out + 0,v0);
  uint64_pack(out + 8,v1);
  return 0;
}
