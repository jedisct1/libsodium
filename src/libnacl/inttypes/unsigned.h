#define DOIT(bits,target) \
int main() \
{ \
  target x; \
  int i; \
 \
  x = 1; \
  for (i = 0;i < bits;++i) { \
    if (x == 0) return 100; \
    x += x; \
  } \
  if (x != 0) return 100; \
  x -= 1; \
  if (x < 0) return 100; \
 \
  return 0; \
}
