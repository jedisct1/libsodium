#include <stdio.h>
#include <sys/types.h>
#include "osfreq.c"

long long cpucycles_amd64cpuinfo(void)
{
  unsigned long long result;
  asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");
  return result;
}

long long cpucycles_amd64cpuinfo_persecond(void)
{
  return osfreq();
}
