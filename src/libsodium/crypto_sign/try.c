/*
 * crypto_sign/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "randombytes.h"
#include "crypto_sign.h"
#include "windows/windows-quirks.h"

#define MAXTEST_BYTES 10000
#define TUNE_BYTES 1536

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_sign_IMPLEMENTATION;

static unsigned char *pk;
static unsigned char *sk;
static unsigned char *m; unsigned long long mlen;
static unsigned char *sm; unsigned long long smlen;
static unsigned char *t; unsigned long long tlen;

void preallocate(void)
{
#ifdef RAND_R_PRNG_NOT_SEEDED
  RAND_status();
#endif
}

void allocate(void)
{
  pk = alignedcalloc(crypto_sign_PUBLICKEYBYTES);
  sk = alignedcalloc(crypto_sign_SECRETKEYBYTES);
  m = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
  sm = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
  t = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
}

void predoit(void)
{
  crypto_sign_keypair(pk,sk);
  mlen = TUNE_BYTES;
  smlen = 0;
  randombytes(m,mlen);
  crypto_sign(sm,&smlen,m,mlen,sk);
}

void doit(void)
{
  crypto_sign_open(t,&tlen,sm,smlen,pk);
}

char checksum[crypto_sign_BYTES * 2 + 1];

const char *checksum_compute(void)
{
  long long mlen;
  long long i;
  long long j;

  if (crypto_sign_keypair(pk,sk) != 0) return "crypto_sign_keypair returns nonzero";
  for (mlen = 0;mlen < MAXTEST_BYTES;mlen += 1 + (mlen / 16)) {
    if (crypto_sign(sm,&smlen,m,mlen,sk) != 0) return "crypto_sign returns nonzero";
    if (crypto_sign_open(t,&tlen,sm,smlen,pk) != 0) return "crypto_sign_open returns nonzero";
    if (tlen != mlen) return "crypto_sign_open does not match length";
    for (i = 0;i < tlen;++i)
      if (t[i] != m[i])
        return "crypto_sign_open does not match contents";

    j = rand() % smlen;
    sm[j] ^= 1;
    if (crypto_sign_open(t,&tlen,sm,smlen,pk) == 0) {
      if (tlen != mlen) return "crypto_sign_open allows trivial forgery of length";
      for (i = 0;i < tlen;++i)
        if (t[i] != m[i])
          return "crypto_sign_open allows trivial forgery of contents";
    }
    sm[j] ^= 1;

  }

  /* do some long-term checksum */
  checksum[0] = 0;
  return 0;
}
