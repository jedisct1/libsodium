#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "osfreq.c"

static long myround(double u)
{
  long result = u;
  while (result + 0.5 < u) result += 1;
  while (result - 0.5 > u) result -= 1;
  return result;
}

static long long microseconds(void)
{
  struct timeval t;
  gettimeofday(&t,(struct timezone *) 0);
  return t.tv_sec * (long long) 1000000 + t.tv_usec;
}

static int tbshift = 0;

static long long timebase(void)
{
  unsigned long high;
  unsigned long low;
  unsigned long newhigh;
  unsigned long long result;
  asm volatile(
    "7:mftbu %0;mftb %1;mftbu %2;cmpw %0,%2;bne 7b"
    : "=r" (high), "=r" (low), "=r" (newhigh)
  );
  result = high;
  result <<= 32;
  result |= low;
  return result >> tbshift;
}

static double cpufrequency = 0;
static long tbcycles = 0;

static double guesstbcycles(void)
{
  long long tb0; long long us0;
  long long tb1; long long us1;

  tb0 = timebase();
  us0 = microseconds();
  do {
    tb1 = timebase();
    us1 = microseconds();
  } while (us1 - us0 < 10000 || tb1 - tb0 < 1000);
  if (tb1 <= tb0) return 0;
  tb1 -= tb0;
  us1 -= us0;
  return (cpufrequency * 0.000001 * (double) us1) / (double) tb1;
}

static void init(void)
{
  int loop;
  double guess1;
  double guess2;

  cpufrequency = osfreq();
  if (!cpufrequency) return;

  for (tbshift = 0;tbshift < 10;++tbshift) {
    for (loop = 0;loop < 100;++loop) {
      guess1 = guesstbcycles();
      guess2 = guesstbcycles();
      tbcycles = myround(guess1);
      if (guess1 - tbcycles > 0.1) continue;
      if (tbcycles - guess1 > 0.1) continue;
      if (guess2 - tbcycles > 0.1) continue;
      if (tbcycles - guess2 > 0.1) continue;
      return;
    }
  }
  tbcycles = 0;
}

long long cpucycles_powerpccpuinfo(void)
{
  if (!tbcycles) init();
  return timebase() * tbcycles;
}

long long cpucycles_powerpccpuinfo_persecond(void)
{
  if (!tbcycles) init();
  return cpufrequency;
}
