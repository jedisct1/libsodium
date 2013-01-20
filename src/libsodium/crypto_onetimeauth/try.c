/*
 * crypto_onetimeauth/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_hash_sha256.h"
#include "crypto_onetimeauth.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_onetimeauth_IMPLEMENTATION;

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
  h = alignedcalloc(crypto_onetimeauth_BYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  k = alignedcalloc(crypto_onetimeauth_KEYBYTES);
  h2 = alignedcalloc(crypto_onetimeauth_BYTES);
  m2 = alignedcalloc(MAXTEST_BYTES + crypto_onetimeauth_BYTES);
  k2 = alignedcalloc(crypto_onetimeauth_KEYBYTES + crypto_onetimeauth_BYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_onetimeauth(h,m,TUNE_BYTES,k);
  crypto_onetimeauth_verify(h,m,TUNE_BYTES,k);
}

char checksum[crypto_onetimeauth_BYTES * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  for (i = 0;i < CHECKSUM_BYTES;++i) {
    long long mlen = i;
    long long klen = crypto_onetimeauth_KEYBYTES;
    long long hlen = crypto_onetimeauth_BYTES;

    for (j = -16;j < 0;++j) h[j] = random();
    for (j = -16;j < 0;++j) k[j] = random();
    for (j = -16;j < 0;++j) m[j] = random();
    for (j = hlen;j < hlen + 16;++j) h[j] = random();
    for (j = klen;j < klen + 16;++j) k[j] = random();
    for (j = mlen;j < mlen + 16;++j) m[j] = random();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

    if (crypto_onetimeauth(h,m,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";

    for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_onetimeauth overwrites k";
    for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_onetimeauth overwrites m";
    for (j = -16;j < 0;++j) if (h[j] != h2[j]) return "crypto_onetimeauth writes before output";
    for (j = hlen;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_onetimeauth writes after output";

    for (j = -16;j < 0;++j) h[j] = random();
    for (j = -16;j < 0;++j) k[j] = random();
    for (j = -16;j < 0;++j) m[j] = random();
    for (j = hlen;j < hlen + 16;++j) h[j] = random();
    for (j = klen;j < klen + 16;++j) k[j] = random();
    for (j = mlen;j < mlen + 16;++j) m[j] = random();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

    if (crypto_onetimeauth(m2,m2,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";
    for (j = 0;j < hlen;++j) if (m2[j] != h[j]) return "crypto_onetimeauth does not handle m overlap";
    for (j = 0;j < hlen;++j) m2[j] = m[j];
    if (crypto_onetimeauth(k2,m2,mlen,k2) != 0) return "crypto_onetimeauth returns nonzero";
    for (j = 0;j < hlen;++j) if (k2[j] != h[j]) return "crypto_onetimeauth does not handle k overlap";
    for (j = 0;j < hlen;++j) k2[j] = k[j];

    if (crypto_onetimeauth_verify(h,m,mlen,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

    for (j = -16;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_onetimeauth overwrites h";
    for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_onetimeauth overwrites k";
    for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_onetimeauth overwrites m";

    crypto_hash_sha256(h2,h,hlen);
    for (j = 0;j < klen;++j) k[j] ^= h2[j % 32];
    if (crypto_onetimeauth(h,m,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";
    if (crypto_onetimeauth_verify(h,m,mlen,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

    crypto_hash_sha256(h2,h,hlen);
    for (j = 0;j < mlen;++j) m[j] ^= h2[j % 32];
    m[mlen] = h2[0];
  }
  if (crypto_onetimeauth(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_onetimeauth returns nonzero";
  if (crypto_onetimeauth_verify(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

  for (i = 0;i < crypto_onetimeauth_BYTES;++i) {
    checksum[2 * i] = "0123456789abcdef"[15 & (h[i] >> 4)];
    checksum[2 * i + 1] = "0123456789abcdef"[15 & h[i]];
  }
  checksum[2 * i] = 0;
  return 0;
}
