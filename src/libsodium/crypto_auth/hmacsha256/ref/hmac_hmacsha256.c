/*
 * 20080913
 * D. J. Bernstein
 * Public domain.
 * */

#include "api.h"
#include "crypto_hashblocks_sha256.h"

#define blocks crypto_hashblocks_sha256

typedef unsigned int uint32;

static const char iv[32] = {
  0x6a,0x09,0xe6,0x67,
  0xbb,0x67,0xae,0x85,
  0x3c,0x6e,0xf3,0x72,
  0xa5,0x4f,0xf5,0x3a,
  0x51,0x0e,0x52,0x7f,
  0x9b,0x05,0x68,0x8c,
  0x1f,0x83,0xd9,0xab,
  0x5b,0xe0,0xcd,0x19,
} ;

int crypto_auth(unsigned char *out,const unsigned char *in,unsigned long long inlen,const unsigned char *k)
{
  unsigned char h[32];
  unsigned char padded[128];
  unsigned long long i;
  unsigned long long bits = 512 + (inlen << 3);

  for (i = 0;i < 32;++i) h[i] = iv[i];

  for (i = 0;i < 32;++i) padded[i] = k[i] ^ 0x36;
  for (i = 32;i < 64;++i) padded[i] = 0x36;

  blocks(h,padded,64);
  blocks(h,in,inlen);
  in += inlen;
  inlen &= 63;
  in -= inlen;

  for (i = 0;i < inlen;++i) padded[i] = in[i];
  padded[inlen] = 0x80;

  if (inlen < 56) {
    for (i = inlen + 1;i < 56;++i) padded[i] = 0;
    padded[56] = bits >> 56;
    padded[57] = bits >> 48;
    padded[58] = bits >> 40;
    padded[59] = bits >> 32;
    padded[60] = bits >> 24;
    padded[61] = bits >> 16;
    padded[62] = bits >> 8;
    padded[63] = bits;
    blocks(h,padded,64);
  } else {
    for (i = inlen + 1;i < 120;++i) padded[i] = 0;
    padded[120] = bits >> 56;
    padded[121] = bits >> 48;
    padded[122] = bits >> 40;
    padded[123] = bits >> 32;
    padded[124] = bits >> 24;
    padded[125] = bits >> 16;
    padded[126] = bits >> 8;
    padded[127] = bits;
    blocks(h,padded,128);
  }

  for (i = 0;i < 32;++i) padded[i] = k[i] ^ 0x5c;
  for (i = 32;i < 64;++i) padded[i] = 0x5c;
  for (i = 0;i < 32;++i) padded[64 + i] = h[i];

  for (i = 0;i < 32;++i) out[i] = iv[i];

  for (i = 32;i < 64;++i) padded[64 + i] = 0;
  padded[64 + 32] = 0x80;
  padded[64 + 62] = 3;

  blocks(out,padded,128);

  return 0;
}
