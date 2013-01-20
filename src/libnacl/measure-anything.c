/*
 * measure-anything.c version 20090223
 * D. J. Bernstein
 * Public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#ifndef _WIN32
# include <sys/resource.h>
#endif
#include "cpucycles.h"
#include "cpuid.h"

typedef int uint32;

static uint32 seed[32] = { 3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5 } ;
static uint32 in[12];
static uint32 out[8];
static int outleft = 0;

#define ROTATE(x,b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i,b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x,b));

static void surf(void)
{
  uint32 t[12]; uint32 x; uint32 sum = 0;
  int r; int i; int loop;

  for (i = 0;i < 12;++i) t[i] = in[i] ^ seed[12 + i];
  for (i = 0;i < 8;++i) out[i] = seed[24 + i];
  x = t[11];
  for (loop = 0;loop < 2;++loop) {
    for (r = 0;r < 16;++r) {
      sum += 0x9e3779b9;
      MUSH(0,5) MUSH(1,7) MUSH(2,9) MUSH(3,13)
      MUSH(4,5) MUSH(5,7) MUSH(6,9) MUSH(7,13)
      MUSH(8,5) MUSH(9,7) MUSH(10,9) MUSH(11,13)
    }
    for (i = 0;i < 8;++i) out[i] ^= t[i + 4];
  }
}

void randombytes(unsigned char *x,unsigned long long xlen)
{
  while (xlen > 0) {
    if (!outleft) {
      if (!++in[0]) if (!++in[1]) if (!++in[2]) ++in[3];
      surf();
      outleft = 8;
    }
    *x = out[--outleft];
    ++x;
    --xlen;
  }
}

extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void preallocate(void);
extern void allocate(void);
extern void measure(void);

static void printword(const char *s)
{
  if (!*s) putchar('-');
  while (*s) {
    if (*s == ' ') putchar('_');
    else if (*s == '\t') putchar('_');
    else if (*s == '\r') putchar('_');
    else if (*s == '\n') putchar('_');
    else putchar(*s);
    ++s;
  }
  putchar(' ');
}

static void printnum(long long x)
{
  printf("%lld ",x);
}

static void fail(const char *why)
{
  fprintf(stderr,"measure: fatal: %s\n",why);
  exit(111);
}

unsigned char *alignedcalloc(unsigned long long len)
{
  unsigned char *x = (unsigned char *) calloc(1,len + 128);
  if (!x) fail("out of memory");
  /* will never deallocate so shifting is ok */
  x += 63 & (-(unsigned long) x);
  return x;
}

static long long cyclespersecond;

static void printimplementations(void)
{
  int i;

  printword("implementation");
  printword(primitiveimplementation);
  printword(implementationversion);
  printf("\n"); fflush(stdout);

  for (i = 0;sizenames[i];++i) {
    printword(sizenames[i]);
    printnum(sizes[i]);
    printf("\n"); fflush(stdout);
  }

  printword("cpuid");
  printword(cpuid);
  printf("\n"); fflush(stdout);

  printword("cpucycles_persecond");
  printnum(cyclespersecond);
  printf("\n"); fflush(stdout);

  printword("cpucycles_implementation");
  printword(cpucycles_implementation);
  printf("\n"); fflush(stdout);

  printword("compiler");
  printword(COMPILER);
#if defined(__VERSION__) && !defined(__ICC)
  printword(__VERSION__);
#elif defined(__xlc__)
  printword(__xlc__);
#elif defined(__ICC)
  {
    char buf[256];

    sprintf(buf, "%d.%d.%d", __ICC/100, __ICC%100,
            __INTEL_COMPILER_BUILD_DATE);
    printword(buf);
  }
#elif defined(__PGIC__)
  {
    char buf[256];

    sprintf(buf, "%d.%d.%d", __PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__);
    printword(buf);
  }
#elif defined(__SUNPRO_C)
  {
    char buf[256];
    int major, minor, micro;

    micro = __SUNPRO_C & 0xf;
    minor = (__SUNPRO_C >> 4) & 0xf;
    major = (__SUNPRO_C >> 8) & 0xf;

    if (micro)
      sprintf(buf, "%d.%d.%d", major, minor, micro);
    else
      sprintf(buf, "%d.%d", major, minor);
    printword(buf);
  }
#else
  printword("unknown compiler version");
#endif
  printf("\n"); fflush(stdout);
}

void printentry(long long mbytes,const char *measuring,long long *m,long long mlen)
{
  long long i;
  long long j;
  long long belowj;
  long long abovej;

  printword(measuring);
  if (mbytes >= 0) printnum(mbytes); else printword("");
  if (mlen > 0) {
    for (j = 0;j + 1 < mlen;++j) {
      belowj = 0;
      for (i = 0;i < mlen;++i) if (m[i] < m[j]) ++belowj;
      abovej = 0;
      for (i = 0;i < mlen;++i) if (m[i] > m[j]) ++abovej;
      if (belowj * 2 < mlen && abovej * 2 < mlen) break;
    }
    printnum(m[j]);
    if (mlen > 1) {
      for (i = 0;i < mlen;++i) printnum(m[i]);
    }
  }
  printf("\n"); fflush(stdout);
}

void limits()
{
#ifdef RLIM_INFINITY
  struct rlimit r;
  r.rlim_cur = 0;
  r.rlim_max = 0;
#ifdef RLIMIT_NOFILE
  setrlimit(RLIMIT_NOFILE,&r);
#endif
#ifdef RLIMIT_NPROC
  setrlimit(RLIMIT_NPROC,&r);
#endif
#ifdef RLIMIT_CORE
  setrlimit(RLIMIT_CORE,&r);
#endif
#endif
}

int main()
{
  cyclespersecond = cpucycles_persecond();
  preallocate();
  limits();
  printimplementations();
  allocate();
  measure();
  return 0;
}
