#include <stdio.h>

int main()
{
  char ch;
  int loop = 0;
  while (scanf("%c",&ch) == 1) {
    printf("0x%02x,",255 & (int) ch);
    if (++loop == 16) {
      loop = 0;
      printf("\n");
    }
  }
  printf("0x00\n");
  return 0;
}
