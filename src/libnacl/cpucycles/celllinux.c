#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <spu_mfcio.h>

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

static long long timebase(void)
{
  unsigned long long result;
  result = -spu_read_decrementer();
  return 0xffffffff & result;
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

  spu_write_decrementer(0xffffffff);

  cpufrequency = 3192000000.0;

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
  tbcycles = 0;
}

long long cpucycles_celllinux(void)
{
  if (!tbcycles) init();
  return timebase() * tbcycles;
}

long long cpucycles_celllinux_persecond(void)
{
  if (!tbcycles) init();
  return cpufrequency;
}
