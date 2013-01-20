#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>

long long cpucycles_x86cpuspeed(void)
{
  long long result;
  asm volatile(".byte 15;.byte 49" : "=A" (result));
  return result;
}

long long cpucycles_x86cpuspeed_persecond(void)
{
  int oid[2];
  int val;
  size_t size;
  oid[0] = CTL_HW;
  oid[1] = HW_CPUSPEED;
  size = sizeof val;
  if (sysctl(oid,2,&val,&size,0,0) == -1) return 0;
  if (size != sizeof val) return 0;
  return val * 1000000LL;
}
