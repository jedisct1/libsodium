/*
 * crypto_auth/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_hash_sha256.h"
#include "crypto_auth.h"
#include "utils.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_auth_IMPLEMENTATION;

#define MAXTEST_BYTES 10000
#define CHECKSUM_BYTES 4096
#define TUNE_BYTES 1536

static unsigned char *h;
static unsigned char *m;
static unsigned char *k;
static unsigned char *h2;
static unsigned char *m2;
static unsigned char *k2;

void preallocate(void)
{
}

void allocate(void)
{
  h = alignedcalloc(crypto_auth_BYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  k = alignedcalloc(crypto_auth_KEYBYTES);
  h2 = alignedcalloc(crypto_auth_BYTES);
  m2 = alignedcalloc(MAXTEST_BYTES + crypto_auth_BYTES);
  k2 = alignedcalloc(crypto_auth_KEYBYTES + crypto_auth_BYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_auth(h,m,TUNE_BYTES,k);
  crypto_auth_verify(h,m,TUNE_BYTES,k);
}

char checksum[crypto_auth_BYTES * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  for (i = 0;i < CHECKSUM_BYTES;++i) {
    long long mlen = i;
    long long klen = crypto_auth_KEYBYTES;
    long long hlen = crypto_auth_BYTES;

    for (j = -16;j < 0;++j) h[j] = rand();
    for (j = -16;j < 0;++j) k[j] = rand();
    for (j = -16;j < 0;++j) m[j] = rand();
    for (j = hlen;j < hlen + 16;++j) h[j] = rand();
    for (j = klen;j < klen + 16;++j) k[j] = rand();
    for (j = mlen;j < mlen + 16;++j) m[j] = rand();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

    if (crypto_auth(h,m,mlen,k) != 0) return "crypto_auth returns nonzero";

    for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_auth overwrites k";
    for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_auth overwrites m";
    for (j = -16;j < 0;++j) if (h[j] != h2[j]) return "crypto_auth writes before output";
    for (j = hlen;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_auth writes after output";

    for (j = -16;j < 0;++j) h[j] = rand();
    for (j = -16;j < 0;++j) k[j] = rand();
    for (j = -16;j < 0;++j) m[j] = rand();
    for (j = hlen;j < hlen + 16;++j) h[j] = rand();
    for (j = klen;j < klen + 16;++j) k[j] = rand();
    for (j = mlen;j < mlen + 16;++j) m[j] = rand();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

    if (crypto_auth(m2,m2,mlen,k) != 0) return "crypto_auth returns nonzero";
    for (j = 0;j < hlen;++j) if (m2[j] != h[j]) return "crypto_auth does not handle m overlap";
    for (j = 0;j < hlen;++j) m2[j] = m[j];
    if (crypto_auth(k2,m2,mlen,k2) != 0) return "crypto_auth returns nonzero";
    for (j = 0;j < hlen;++j) if (k2[j] != h[j]) return "crypto_auth does not handle k overlap";
    for (j = 0;j < hlen;++j) k2[j] = k[j];

    if (crypto_auth_verify(h,m,mlen,k) != 0) return "crypto_auth_verify returns nonzero";

    for (j = -16;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_auth overwrites h";
    for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_auth overwrites k";
    for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_auth overwrites m";

    crypto_hash_sha256(h2,h,hlen);
    for (j = 0;j < klen;++j) k[j] ^= h2[j % 32];
    if (crypto_auth(h,m,mlen,k) != 0) return "crypto_auth returns nonzero";
    if (crypto_auth_verify(h,m,mlen,k) != 0) return "crypto_auth_verify returns nonzero";

    crypto_hash_sha256(h2,h,hlen);
    for (j = 0;j < mlen;++j) m[j] ^= h2[j % 32];
    m[mlen] = h2[0];
  }
  if (crypto_auth(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_auth returns nonzero";
  if (crypto_auth_verify(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_auth_verify returns nonzero";

  sodium_bin2hex(checksum, sizeof checksum, h, crypto_auth_BYTES);

  return 0;
}
