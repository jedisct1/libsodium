#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_scalarmult.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_scalarmult_IMPLEMENTATION;
const char *implementationversion = crypto_scalarmult_VERSION;
const char *sizenames[] = { "outputbytes", "scalarbytes", 0 };
const long long sizes[] = { crypto_scalarmult_BYTES, crypto_scalarmult_SCALARBYTES };

static unsigned char *m;
static unsigned char *n;
static unsigned char *p;
static unsigned char *q;

void preallocate(void)
{
}

void allocate(void)
{
  m = alignedcalloc(crypto_scalarmult_SCALARBYTES);
  n = alignedcalloc(crypto_scalarmult_SCALARBYTES);
  p = alignedcalloc(crypto_scalarmult_BYTES);
  q = alignedcalloc(crypto_scalarmult_BYTES);
}

#define TIMINGS 63
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;

  for (loop = 0;loop < LOOPS;++loop) {
    randombytes(m,crypto_scalarmult_SCALARBYTES);
    randombytes(n,crypto_scalarmult_SCALARBYTES);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_scalarmult_base(p,m);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"base_cycles",cycles,TIMINGS);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_scalarmult(q,n,p);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"cycles",cycles,TIMINGS);
  }
}
