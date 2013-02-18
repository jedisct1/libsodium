#include "nameparse.h"

int nameparse(unsigned char *s,const char *x)
{
  long long pos;
  long long j;
  if (!x) return 0;
  for (pos = 0;pos < 256;++pos) s[pos] = 0;
  pos = 0;
  while (*x) {
    if (*x == '.') { ++x; continue; }
    for (j = 0;x[j];++j) if (x[j] == '.') break;
    if (j > 63) return 0;
    if (pos < 0 || pos >= 256) return 0; s[pos++] = j;
    while (j > 0) { if (pos < 0 || pos >= 256) return 0; s[pos++] = *x++; --j; }
  }
  if (pos < 0 || pos >= 256) return 0; s[pos++] = 0;
  return 1;
}
