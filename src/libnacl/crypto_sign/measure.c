#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_sign.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_sign_IMPLEMENTATION;
const char *implementationversion = crypto_sign_VERSION;
const char *sizenames[] = { "outputbytes", "publickeybytes", "secretkeybytes", 0 };
const long long sizes[] = { crypto_sign_BYTES, crypto_sign_PUBLICKEYBYTES, crypto_sign_SECRETKEYBYTES };

#define MAXTEST_BYTES 100000

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

#define TIMINGS 31
static long long cycles[TIMINGS + 1];
static long long bytes[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;

  for (loop = 0;loop < LOOPS;++loop) {
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_sign_keypair(pk,sk);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"keypair_cycles",cycles,TIMINGS);

    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / 4) {
      randombytes(m,mlen);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        bytes[i] = crypto_sign(sm,&smlen,m,mlen,sk);
	if (bytes[i] == 0) bytes[i] = smlen;
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);
      printentry(mlen,"bytes",bytes,TIMINGS);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        bytes[i] = crypto_sign_open(t,&tlen,sm,smlen,pk);
	if (bytes[i] == 0) bytes[i] = tlen;
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"open_cycles",cycles,TIMINGS);
      printentry(mlen,"open_bytes",bytes,TIMINGS);
    }
  }
}
