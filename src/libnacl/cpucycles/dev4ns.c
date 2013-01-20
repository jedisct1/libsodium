#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

static int fddev = -1;
static int prev[3];
static unsigned long long prevcycles = 0;
static int now[3];
static long long cyclespersec = 0;

static void readdev(unsigned int *result)
{
  if (read(fddev,result,12) == 12) return;
  result[0] = result[1] = result[2] = 0;
}

long long cpucycles_dev4ns(void)
{
  unsigned long long delta4;
  int deltan;
  int deltas;
  unsigned long long guesscycles;

  if (fddev == -1) {
    fddev = open("/dev/cpucycles4ns",O_RDONLY);
    readdev(prev);
  }

  readdev(now);
  delta4 = (unsigned int) (now[0] - prev[0]); /* unsigned change in number of cycles mod 2^32 */
  deltan = now[1] - prev[1]; /* signed change in number of nanoseconds mod 10^9 */
  deltas = now[2] - prev[2]; /* signed change in number of seconds */
  if ((deltas == 0 && deltan < 200000000) || (deltas == 1 && deltan < -800000000))
    return prevcycles + delta4;

  prev[0] = now[0];
  prev[1] = now[1];
  prev[2] = now[2];

  if ((deltas == 0 && deltan < 300000000) || (deltas == 1 && deltan < -700000000)) {
    // actual number of cycles cannot have increased by 2^32 in <0.3ms
    cyclespersec = 1000000000 * (unsigned long long) delta4;
    cyclespersec /= deltan + 1000000000 * (long long) deltas;
  } else {
    guesscycles = deltas * cyclespersec;
    guesscycles += (deltan * cyclespersec) / 1000000000;
    while (delta4 + 2147483648ULL < guesscycles) delta4 += 4294967296ULL;
    /* XXX: could do longer-term extrapolation here */
  }

  prevcycles += delta4;
  return prevcycles;
}

long long cpucycles_dev4ns_persecond(void)
{
  while (!cyclespersec) cpucycles_dev4ns();
  return cyclespersec;
}
