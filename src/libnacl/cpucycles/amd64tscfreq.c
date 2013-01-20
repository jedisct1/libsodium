#include <stdio.h>
#include <sys/types.h>

long long cpucycles_amd64tscfreq(void)
{
  unsigned long long result;
  asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");
  return result;
}

long long cpucycles_amd64tscfreq_persecond(void)
{
  long result = 0;
  size_t resultlen = sizeof(long);
  sysctlbyname("machdep.tsc_freq",&result,&resultlen,0,0);
  return result;
}
