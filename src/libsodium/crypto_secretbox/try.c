/*
 * crypto_secretbox/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_secretbox.h"
#include "utils.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_secretbox_IMPLEMENTATION;

#define MAXTEST_BYTES 10000
#define CHECKSUM_BYTES 4096
#define TUNE_BYTES 1536

static unsigned char *k;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;
static unsigned char *t;
static unsigned char *k2;
static unsigned char *n2;
static unsigned char *m2;
static unsigned char *c2;
static unsigned char *t2;

#define klen crypto_secretbox_KEYBYTES
#define nlen crypto_secretbox_NONCEBYTES

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(klen);
  n = alignedcalloc(nlen);
  m = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  c = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  t = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  k2 = alignedcalloc(klen);
  n2 = alignedcalloc(nlen);
  m2 = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  c2 = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  t2 = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_secretbox(c,m,TUNE_BYTES + crypto_secretbox_ZEROBYTES,n,k);
  crypto_secretbox_open(t,c,TUNE_BYTES + crypto_secretbox_ZEROBYTES,n,k);
}

char checksum[klen * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  for (j = 0;j < crypto_secretbox_ZEROBYTES;++j) m[j] = 0;

  for (i = 0;i < CHECKSUM_BYTES;++i) {
    long long mlen = i + crypto_secretbox_ZEROBYTES;
    long long tlen = i + crypto_secretbox_ZEROBYTES;
    long long clen = i + crypto_secretbox_ZEROBYTES;

    for (j = -16;j < 0;++j) k[j] = rand();
    for (j = -16;j < 0;++j) n[j] = rand();
    for (j = -16;j < 0;++j) m[j] = rand();
    for (j = klen;j < klen + 16;++j) k[j] = rand();
    for (j = nlen;j < nlen + 16;++j) n[j] = rand();
    for (j = mlen;j < mlen + 16;++j) m[j] = rand();
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < nlen + 16;++j) n2[j] = n[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];
    for (j = -16;j < clen + 16;++j) c2[j] = c[j] = rand();

    if (crypto_secretbox(c,m,mlen,n,k) != 0) return "crypto_secretbox returns nonzero";

    for (j = -16;j < mlen + 16;++j) if (m2[j] != m[j]) return "crypto_secretbox overwrites m";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_secretbox overwrites n";
    for (j = -16;j < klen + 16;++j) if (k2[j] != k[j]) return "crypto_secretbox overwrites k";
    for (j = -16;j < 0;++j) if (c2[j] != c[j]) return "crypto_secretbox writes before output";
    for (j = clen;j < clen + 16;++j) if (c2[j] != c[j]) return "crypto_secretbox writes after output";
    for (j = 0;j < crypto_secretbox_BOXZEROBYTES;++j)
      if (c[j] != 0) return "crypto_secretbox does not clear extra bytes";

    for (j = -16;j < 0;++j) c[j] = rand();
    for (j = clen;j < clen + 16;++j) c[j] = rand();
    for (j = -16;j < clen + 16;++j) c2[j] = c[j];
    for (j = -16;j < tlen + 16;++j) t2[j] = t[j] = rand();

    if (crypto_secretbox_open(t,c,clen,n,k) != 0) return "crypto_secretbox_open returns nonzero";

    for (j = -16;j < clen + 16;++j) if (c2[j] != c[j]) return "crypto_secretbox_open overwrites c";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_secretbox_open overwrites n";
    for (j = -16;j < klen + 16;++j) if (k2[j] != k[j]) return "crypto_secretbox_open overwrites k";
    for (j = -16;j < 0;++j) if (t2[j] != t[j]) return "crypto_secretbox_open writes before output";
    for (j = tlen;j < tlen + 16;++j) if (t2[j] != t[j]) return "crypto_secretbox_open writes after output";
    for (j = 0;j < crypto_secretbox_ZEROBYTES;++j)
      if (t[j] != 0) return "crypto_secretbox_open does not clear extra bytes";

    for (j = 0;j < i;++j) if (t[j] != m[j]) return "plaintext does not match";

    for (j = 0;j < i;++j)
      k[j % klen] ^= c[j + crypto_secretbox_BOXZEROBYTES];
    crypto_secretbox(c,m,mlen,n,k);
    for (j = 0;j < i;++j)
      n[j % nlen] ^= c[j + crypto_secretbox_BOXZEROBYTES];
    crypto_secretbox(c,m,mlen,n,k);
    if (i == 0) m[crypto_secretbox_ZEROBYTES + 0] = 0;
    m[crypto_secretbox_ZEROBYTES + i] = m[crypto_secretbox_ZEROBYTES + 0];
    for (j = 0;j < i;++j)
      m[j + crypto_secretbox_ZEROBYTES] ^= c[j + crypto_secretbox_BOXZEROBYTES];
  }

  sodium_bin2hex(checksum, sizeof checksum, k, klen);

  return 0;
}
