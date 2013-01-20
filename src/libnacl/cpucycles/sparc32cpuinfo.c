#include <stdio.h>
#include <sys/types.h>
#include "osfreq.c"

long long cpucycles_sparc32cpuinfo(void)
{
  long long result;
  asm volatile(".word 2202075136; .word 2570088480; srl %%g1,0,%L0; mov %%o4,%H0"
    : "=r" (result) : : "g1","o4");
  return result;
}

long long cpucycles_sparc32cpuinfo_persecond(void)
{
  return osfreq();
}
