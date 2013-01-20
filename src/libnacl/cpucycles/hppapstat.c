#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/pstat.h>
#include <machine/inline.h>

long long cpucycles_hppapstat(void)
{
  register long long result;
  _MFCTL(16,result);
  return result;
}

long long cpucycles_hppapstat_persecond(void)
{
  struct pst_processor pst;
  union pstun pu;
  double result;

  pu.pst_processor = &pst;
  if (pstat(PSTAT_PROCESSOR,pu,sizeof(pst),1,0) < 0) return 0;
  result = pst.psp_iticksperclktick;
  result *= (double) sysconf(_SC_CLK_TCK);
  return result;
}
