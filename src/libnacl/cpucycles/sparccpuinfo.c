#include <stdio.h>
#include <sys/types.h>
#include "osfreq.c"

long long cpucycles_sparccpuinfo(void)
{
  long long result;
  asm volatile("rd %%tick,%0" : "=r" (result));
  return result;
}

long long cpucycles_sparccpuinfo_persecond(void)
{
  return osfreq();
}
