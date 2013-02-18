#include "hexparse.h"

static int hexdigit(char x)
{
  if (x >= '0' && x <= '9') return x - '0';
  if (x >= 'a' && x <= 'f') return 10 + (x - 'a');
  if (x >= 'A' && x <= 'F') return 10 + (x - 'A');
  return -1;
}

int hexparse(unsigned char *y,long long len,const char *x)
{
  if (!x) return 0;
  while (len > 0) {
    int digit0;
    int digit1;
    digit0 = hexdigit(x[0]); if (digit0 == -1) return 0;
    digit1 = hexdigit(x[1]); if (digit1 == -1) return 0;
    *y++ = digit1 + 16 * digit0;
    --len;
    x += 2;
  }
  if (x[0]) return 0;
  return 1;
}
