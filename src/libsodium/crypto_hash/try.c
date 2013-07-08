/*
 * crypto_hash/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_hash.h"
#include "utils.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_hash_IMPLEMENTATION;

#define MAXTEST_BYTES (10000 + crypto_hash_BYTES)
#define CHECKSUM_BYTES 4096
#define TUNE_BYTES 1536

static unsigned char *h;
static unsigned char *h2;
static unsigned char *m;
static unsigned char *m2;

void preallocate(void)
{
}

void allocate(void)
{
  h = alignedcalloc(crypto_hash_BYTES);
  h2 = alignedcalloc(crypto_hash_BYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  m2 = alignedcalloc(MAXTEST_BYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_hash(h,m,TUNE_BYTES);
}

char checksum[crypto_hash_BYTES * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  for (i = 0;i < CHECKSUM_BYTES;++i) {
    long long hlen = crypto_hash_BYTES;
    long long mlen = i;
    for (j = -16;j < 0;++j) h[j] = rand();
    for (j = hlen;j < hlen + 16;++j) h[j] = rand();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < 0;++j) m[j] = rand();
    for (j = mlen;j < mlen + 16;++j) m[j] = rand();
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];
    if (crypto_hash(h,m,mlen) != 0) return "crypto_hash returns nonzero";
    for (j = -16;j < mlen + 16;++j) if (m2[j] != m[j]) return "crypto_hash writes to input";
    for (j = -16;j < 0;++j) if (h2[j] != h[j]) return "crypto_hash writes before output";
    for (j = hlen;j < hlen + 16;++j) if (h2[j] != h[j]) return "crypto_hash writes after output";
    if (crypto_hash(m2,m2,mlen) != 0) return "crypto_hash returns nonzero";
    for (j = 0;j < hlen;++j) if (m2[j] != h[j]) return "crypto_hash does not handle overlap";
    for (j = 0;j < mlen;++j) m[j] ^= h[j % hlen];
    m[mlen] = h[0];
  }
  if (crypto_hash(h,m,CHECKSUM_BYTES) != 0) return "crypto_hash returns nonzero";

  sodium_bin2hex(checksum, sizeof checksum, h, crypto_hash_BYTES);

  return 0;
}
