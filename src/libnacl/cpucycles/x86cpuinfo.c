#include <stdio.h>
#include <sys/types.h>
#include "osfreq.c"

long long cpucycles_x86cpuinfo(void)
{
  long long result;
  asm volatile(".byte 15;.byte 49" : "=A" (result));
  return result;
}

long long cpucycles_x86cpuinfo_persecond(void)
{
  return osfreq();
}
