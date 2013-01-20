#include "randombytes-impl.h"

unsigned char x[65536];
unsigned long long freq[256];

int main()
{
  unsigned long long i;

  randombytes(x,sizeof x);
  for (i = 0;i < 256;++i) freq[i] = 0;
  for (i = 0;i < sizeof x;++i) ++freq[255 & (int) x[i]];
  for (i = 0;i < 256;++i) if (!freq[i]) return 111;
  return 0;
}
