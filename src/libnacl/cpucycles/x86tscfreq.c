#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>

long long cpucycles_x86tscfreq(void)
{
  long long result;
  asm volatile(".byte 15;.byte 49" : "=A" (result));
  return result;
}

long long cpucycles_x86tscfreq_persecond(void)
{
  long result = 0;
  size_t resultlen = sizeof(long);
  sysctlbyname("machdep.tsc_freq",&result,&resultlen,0,0);
  return result;
}
