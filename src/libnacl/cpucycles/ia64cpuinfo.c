#include <stdio.h>
#include <sys/types.h>
#include "osfreq.c"

long long cpucycles_ia64cpuinfo(void)
{
  long long result;
  asm volatile("mov %0=ar.itc" : "=r"(result));
  return result;
}

long long cpucycles_ia64cpuinfo_persecond(void)
{
  return osfreq();
}
