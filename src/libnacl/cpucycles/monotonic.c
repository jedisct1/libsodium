#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>

static double cpufrequency = 0;

static void init(void)
{
  long result = 0; size_t resultlen = sizeof(long);
  sysctlbyname("machdep.tsc_freq",&result,&resultlen,0,0);
  cpufrequency = result;
}

long long cpucycles_monotonic(void)
{
  double result;
  struct timespec t;
  if (!cpufrequency) init();
  clock_gettime(CLOCK_MONOTONIC,&t);
  result = t.tv_nsec;
  result *= 0.000000001;
  result += (double) t.tv_sec;
  result *= cpufrequency;
  return result;
}

long long cpucycles_monotonic_persecond(void)
{
  if (!cpufrequency) init();
  return cpufrequency;
}
