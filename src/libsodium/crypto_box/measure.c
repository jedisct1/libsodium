#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_box.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_box_IMPLEMENTATION;
const char *implementationversion = crypto_box_VERSION;
const char *sizenames[] = { "publickeybytes", "secretkeybytes", "beforenmbytes", "noncebytes", "zerobytes", "boxzerobytes", 0 };
const long long sizes[] = { crypto_box_PUBLICKEYBYTES, crypto_box_SECRETKEYBYTES, crypto_box_BEFORENMBYTES, crypto_box_NONCEBYTES, crypto_box_ZEROBYTES, crypto_box_BOXZEROBYTES };

#define MAXTEST_BYTES 4096

static unsigned char *ska;
static unsigned char *pka;
static unsigned char *skb;
static unsigned char *pkb;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;
static unsigned char *sa;
static unsigned char *sb;

void preallocate(void)
{
}

void allocate(void)
{
  ska = alignedcalloc(crypto_box_SECRETKEYBYTES);
  pka = alignedcalloc(crypto_box_PUBLICKEYBYTES);
  skb = alignedcalloc(crypto_box_SECRETKEYBYTES);
  pkb = alignedcalloc(crypto_box_PUBLICKEYBYTES);
  n = alignedcalloc(crypto_box_NONCEBYTES);
  m = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  c = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  sa = alignedcalloc(crypto_box_BEFORENMBYTES);
  sb = alignedcalloc(crypto_box_BEFORENMBYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;
  int mlen;

  for (loop = 0;loop < LOOPS;++loop) {
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_box_keypair(pka,ska);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"keypair_cycles",cycles,TIMINGS);

    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_box_keypair(pkb,skb);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"keypair_cycles",cycles,TIMINGS);

    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_box_beforenm(sa,pkb,ska);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"beforenm_cycles",cycles,TIMINGS);

    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_box_beforenm(sb,pka,skb);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"beforenm_cycles",cycles,TIMINGS);

    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / 8) {
      randombytes(n,crypto_box_NONCEBYTES);
      randombytes(m + crypto_box_ZEROBYTES,mlen);
      randombytes(c,mlen + crypto_box_ZEROBYTES);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box(c,m,mlen + crypto_box_ZEROBYTES,n,pka,skb);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box_open(m,c,mlen + crypto_box_ZEROBYTES,n,pkb,ska);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"open_cycles",cycles,TIMINGS);

      ++c[crypto_box_ZEROBYTES];
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box_open(m,c,mlen + crypto_box_ZEROBYTES,n,pkb,ska);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"forgery_open_cycles",cycles,TIMINGS);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box_afternm(c,m,mlen + crypto_box_ZEROBYTES,n,sb);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"afternm_cycles",cycles,TIMINGS);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box_open_afternm(m,c,mlen + crypto_box_ZEROBYTES,n,sa);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"open_afternm_cycles",cycles,TIMINGS);

      ++c[crypto_box_ZEROBYTES];
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_box_open_afternm(m,c,mlen + crypto_box_ZEROBYTES,n,sa);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"forgery_open_afternm_cycles",cycles,TIMINGS);
    }
  }
}
