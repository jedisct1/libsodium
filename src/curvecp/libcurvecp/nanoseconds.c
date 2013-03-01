#include <sys/time.h>
#include <time.h>
#include "nanoseconds.h"

/* XXX: Y2036 problems; should upgrade to a 128-bit type for this */
/* XXX: nanosecond granularity limits users to 1 terabyte per second */

long long nanoseconds(void)
{
#if defined(CLOCK_REALTIME) && defined(HAVE_CLOCK_GETTIME)
  struct timespec t;
  if (clock_gettime(CLOCK_REALTIME,&t) != 0) return -1;
  return t.tv_sec * 1000000000LL + t.tv_nsec;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000000LL + tv.tv_usec * 1000LL;
#endif
}
