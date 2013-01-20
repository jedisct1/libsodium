/*
 * crypto_core/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_core.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_core_IMPLEMENTATION;

static unsigned char *h;
static unsigned char *n;
static unsigned char *k;
static unsigned char *c;
static unsigned char *h2;
static unsigned char *n2;
static unsigned char *k2;
static unsigned char *c2;

#define hlen crypto_core_OUTPUTBYTES
#define nlen crypto_core_INPUTBYTES
#define klen crypto_core_KEYBYTES
#define clen crypto_core_CONSTBYTES

void preallocate(void)
{
}

void allocate(void)
{
  h = alignedcalloc(hlen);
  n = alignedcalloc(nlen);
  k = alignedcalloc(klen);
  c = alignedcalloc(clen);
  h2 = alignedcalloc(hlen);
  n2 = alignedcalloc(nlen + crypto_core_OUTPUTBYTES);
  k2 = alignedcalloc(klen + crypto_core_OUTPUTBYTES);
  c2 = alignedcalloc(clen + crypto_core_OUTPUTBYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_core(h,n,k,c);
}

static unsigned char newbyte(void)
{
  unsigned long long x;
  long long j;
  x = 8675309;
  for (j = 0;j < hlen;++j) { x += h[j]; x *= x; x += (x >> 31); }
  for (j = 0;j < nlen;++j) { x += n[j]; x *= x; x += (x >> 31); }
  for (j = 0;j < klen;++j) { x += k[j]; x *= x; x += (x >> 31); }
  for (j = 0;j < clen;++j) { x += c[j]; x *= x; x += (x >> 31); }
  for (j = 0;j < 100;++j)  { x +=   j ; x *= x; x += (x >> 31); }
  return x;
}

char checksum[hlen * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  for (i = 0;i < 100;++i) {
    for (j = -16;j < 0;++j) h[j] = random();
    for (j = hlen;j < hlen + 16;++j) h[j] = random();
    for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
    for (j = -16;j < 0;++j) n[j] = random();
    for (j = nlen;j < nlen + 16;++j) n[j] = random();
    for (j = -16;j < nlen + 16;++j) n2[j] = n[j];
    for (j = -16;j < 0;++j) k[j] = random();
    for (j = klen;j < klen + 16;++j) k[j] = random();
    for (j = -16;j < klen + 16;++j) k2[j] = k[j];
    for (j = -16;j < 0;++j) c[j] = random();
    for (j = clen;j < clen + 16;++j) c[j] = random();
    for (j = -16;j < clen + 16;++j) c2[j] = c[j];
    if (crypto_core(h,n,k,c) != 0) return "crypto_core returns nonzero";
    for (j = -16;j < 0;++j) if (h2[j] != h[j]) return "crypto_core writes before output";
    for (j = hlen;j < hlen + 16;++j) if (h2[j] != h[j]) return "crypto_core writes after output";
    for (j = -16;j < klen + 16;++j) if (k2[j] != k[j]) return "crypto_core writes to k";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_core writes to n";
    for (j = -16;j < clen + 16;++j) if (c2[j] != c[j]) return "crypto_core writes to c";

    if (crypto_core(n2,n2,k,c) != 0) return "crypto_core returns nonzero";
    for (j = 0;j < hlen;++j) if (h[j] != n2[j]) return "crypto_core does not handle n overlap";
    for (j = 0;j < hlen;++j) n2[j] = n[j];
    if (crypto_core(k2,n2,k2,c) != 0) return "crypto_core returns nonzero";
    for (j = 0;j < hlen;++j) if (h[j] != k2[j]) return "crypto_core does not handle k overlap";
    for (j = 0;j < hlen;++j) k2[j] = k[j];
    if (crypto_core(c2,n2,k2,c2) != 0) return "crypto_core returns nonzero";
    for (j = 0;j < hlen;++j) if (h[j] != c2[j]) return "crypto_core does not handle c overlap";
    for (j = 0;j < hlen;++j) c2[j] = c[j];

    for (j = 0;j < nlen;++j) n[j] = newbyte();
    if (crypto_core(h,n,k,c) != 0) return "crypto_core returns nonzero";
    for (j = 0;j < klen;++j) k[j] = newbyte();
    if (crypto_core(h,n,k,c) != 0) return "crypto_core returns nonzero";
    for (j = 0;j < clen;++j) c[j] = newbyte();
  }

  for (i = 0;i < hlen;++i) {
    checksum[2 * i] = "0123456789abcdef"[15 & (h[i] >> 4)];
    checksum[2 * i + 1] = "0123456789abcdef"[15 & h[i]];
  }
  checksum[2 * i] = 0;
  return 0;
}
