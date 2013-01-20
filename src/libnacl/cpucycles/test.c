#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "cpucycles-impl.h"

static long long tod(void)
{
  struct timeval t;
  gettimeofday(&t,(struct timezone *) 0);
  return t.tv_sec * (long long) 1000000 + t.tv_usec;
}

long long todstart;
long long todend;
long long cpustart;
long long cpuend;

long long cyclespersecond;
long long cyclespertod;

long long t[1001];

int main()
{
  return 0;
}
