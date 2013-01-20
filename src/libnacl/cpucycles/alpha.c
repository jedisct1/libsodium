/*
cpucycles/alpha.c version 20060316
D. J. Bernstein
Public domain.
*/

#include <time.h>
#include <unistd.h>
#include <sys/time.h>

static long long tod(void)
{
  struct timeval t;
  gettimeofday(&t,(struct timezone *) 0);
  return t.tv_sec * (long long) 1000000 + t.tv_usec;
}

static long long rpcc(void)
{
  unsigned long long t;
  asm volatile("rpcc %0" : "=r"(t));
  return t & 0xffffffff;
}

static long long firstrpcc;
static long long firsttod;
static long long lastrpcc;
static long long lasttod;
static double mhz = 0;

static void init(void)
{
  firstrpcc = rpcc();
  firsttod = tod();

  do {
    lastrpcc = rpcc();
    lasttod = tod();
  } while (lasttod - firsttod < 10000);

  lastrpcc -= firstrpcc; lastrpcc &= 0xffffffff;
  lasttod -= firsttod;

  mhz = (double) lastrpcc / (double) lasttod;
}

long long cpucycles_alpha(void)
{
  double x;
  long long y;

  if (!mhz) init();

  lastrpcc = rpcc();
  lasttod = tod();

  lastrpcc -= firstrpcc; lastrpcc &= 0xffffffff;
  lasttod -= firsttod;

  /* Number of cycles since firstrpcc is lastrpcc + 2^32 y for unknown y. */
  /* Number of microseconds since firsttod is lasttod. */

  x = (lasttod * mhz - lastrpcc) * 0.00000000023283064365386962890625;
  y = x;
  while (x > y + 0.5) y += 1;
  while (x < y - 0.5) y -= 1;

  y *= 4294967296ULL;
  lastrpcc += y;

  mhz = (double) lastrpcc / (double) lasttod;

  return firstrpcc + lastrpcc;
}

long long cpucycles_alpha_persecond(void)
{
  if (!mhz) init();
  return 1000000.0 * mhz;
}
