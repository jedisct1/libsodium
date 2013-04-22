/*
 * crypto_verify/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_verify.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_verify_IMPLEMENTATION;

static unsigned char *x;
static unsigned char *y;

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(crypto_verify_BYTES);
  y = alignedcalloc(crypto_verify_BYTES);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_verify(x,y);
}

static const char *check(void)
{
  int r = crypto_verify(x,y);
  if (r == 0) {
    if (memcmp(x,y,crypto_verify_BYTES)) return "different strings pass verify";
  } else if (r == -1) {
    if (!memcmp(x,y,crypto_verify_BYTES)) return "equal strings fail verify";
  } else {
    return "weird return value from verify";
  }
  return 0;
}

char checksum[2];

const char *checksum_compute(void)
{
  long long tests;
  long long i;
  long long j;
  const char *c;

  for (tests = 0;tests < 100000;++tests) {
    for (i = 0;i < crypto_verify_BYTES;++i) x[i] = rand();
    for (i = 0;i < crypto_verify_BYTES;++i) y[i] = rand();
    c = check(); if (c) return c;
    for (i = 0;i < crypto_verify_BYTES;++i) y[i] = x[i];
    c = check(); if (c) return c;
    y[rand() % crypto_verify_BYTES] = rand();
    c = check(); if (c) return c;
    y[rand() % crypto_verify_BYTES] = rand();
    c = check(); if (c) return c;
    y[rand() % crypto_verify_BYTES] = rand();
    c = check(); if (c) return c;
  }

  checksum[0] = '0';
  checksum[1] = 0;
  return 0;
}
