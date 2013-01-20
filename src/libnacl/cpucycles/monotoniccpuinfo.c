#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include "osfreq.c"

static double cpufrequency = 0;

static void init(void)
{
  cpufrequency = osfreq();
}

long long cpucycles_monotoniccpuinfo(void)
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

long long cpucycles_monotoniccpuinfo_persecond(void)
{
  if (!cpufrequency) init();
  return cpufrequency;
}
