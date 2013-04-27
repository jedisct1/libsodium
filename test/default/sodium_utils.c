#include <stdio.h>
#include <string.h>

#define TEST_NAME "sodium_utils"
#include "cmptest.h"

int main(void)
{
  unsigned char buf1[1000];
  unsigned char buf2[1000];

  randombytes(buf1, sizeof buf1);
  memcpy(buf2, buf1, sizeof buf2);
  printf ("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
  sodium_memzero(buf1, 0U);
  printf ("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
  sodium_memzero(buf1, sizeof buf1 / 2);
  printf ("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
  printf ("%d\n", sodium_memcmp(buf1, buf2, 0U));
  sodium_memzero(buf2, sizeof buf2 / 2);
  printf ("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));

  return 0;
}
