#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_stream.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_stream_IMPLEMENTATION;
const char *implementationversion = crypto_stream_VERSION;
const char *sizenames[] = { "keybytes", "noncebytes", 0 };
const long long sizes[] = { crypto_stream_KEYBYTES, crypto_stream_NONCEBYTES };

#define MAXTEST_BYTES 4096
#ifdef SUPERCOP
#define MGAP 8192
#else
#define MGAP 8
#endif

static unsigned char *k;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(crypto_stream_KEYBYTES);
  n = alignedcalloc(crypto_stream_NONCEBYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  c = alignedcalloc(MAXTEST_BYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;
  int mlen;

  for (loop = 0;loop < LOOPS;++loop) {
    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / MGAP) {
      randombytes(k,crypto_stream_KEYBYTES);
      randombytes(n,crypto_stream_NONCEBYTES);
      randombytes(m,mlen);
      randombytes(c,mlen);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_stream(c,mlen,n,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_stream_xor(c,m,mlen,n,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"xor_cycles",cycles,TIMINGS);
    }
  }
}
