#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>

static double cpufrequency = 0;

static void init(void)
{
  FILE *f;

  f = popen("hinv -c processor | awk '{if ($3==\"MHZ\") print $2*1000000}'","r");
  if (!f) return;
  if (fscanf(f,"%lf",&cpufrequency) < 1) cpufrequency = 0;
  pclose(f);
  if (!cpufrequency) return;
}

long long cpucycles_sgi(void)
{
  double result;
  struct timespec t;
  if (!cpufrequency) init();
  clock_gettime(CLOCK_SGI_CYCLE,&t);
  result = t.tv_nsec;
  result *= 0.000000001;
  result += (double) t.tv_sec;
  result *= cpufrequency;
  return result;
}

long long cpucycles_sgi_persecond(void)
{
  if (!cpufrequency) init();
  return cpufrequency;
}
