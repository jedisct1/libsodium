#include <unistd.h>
#include "writeall.h"
#include "die.h"

void die_9(int e
  ,const char *s0
  ,const char *s1
  ,const char *s2
  ,const char *s3
  ,const char *s4
  ,const char *s5
  ,const char *s6
  ,const char *s7
  ,const char *s8
)
{
  const char *s[9];
  const char *x;
  char buf[1024];
  int buflen = 0;
  int i;

  s[0] = s0;
  s[1] = s1;
  s[2] = s2;
  s[3] = s3;
  s[4] = s4;
  s[5] = s5;
  s[6] = s6;
  s[7] = s7;
  s[8] = s8;
  for (i = 0;i < 9;++i) {
    x = s[i];
    if (!x) continue;
    while (*x) {
      if (buflen == sizeof buf) { writeall(2,buf,buflen); buflen = 0; }
      buf[buflen++] = *x++;
    }
  }
  writeall(2,buf,buflen);
  _exit(e);
}
