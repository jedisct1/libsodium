#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_secretbox.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_secretbox_IMPLEMENTATION;
const char *implementationversion = crypto_secretbox_VERSION;
const char *sizenames[] = { "keybytes", "noncebytes", "zerobytes", "boxzerobytes", 0 };
const long long sizes[] = { crypto_secretbox_KEYBYTES, crypto_secretbox_NONCEBYTES, crypto_secretbox_ZEROBYTES, crypto_secretbox_BOXZEROBYTES };

#define MAXTEST_BYTES 4096

static unsigned char *k;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(crypto_secretbox_KEYBYTES);
  n = alignedcalloc(crypto_secretbox_NONCEBYTES);
  m = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
  c = alignedcalloc(MAXTEST_BYTES + crypto_secretbox_ZEROBYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;
  int mlen;

  for (loop = 0;loop < LOOPS;++loop) {
    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / 8) {
      randombytes(k,crypto_secretbox_KEYBYTES);
      randombytes(n,crypto_secretbox_NONCEBYTES);
      randombytes(m + crypto_secretbox_ZEROBYTES,mlen);
      randombytes(c,mlen + crypto_secretbox_ZEROBYTES);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_secretbox(c,m,mlen + crypto_secretbox_ZEROBYTES,n,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_secretbox_open(m,c,mlen + crypto_secretbox_ZEROBYTES,n,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"open_cycles",cycles,TIMINGS);
      ++c[crypto_secretbox_ZEROBYTES];
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_secretbox_open(m,c,mlen + crypto_secretbox_ZEROBYTES,n,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"forgery_open_cycles",cycles,TIMINGS);
    }
  }
}
