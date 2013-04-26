#include <stdio.h>

#define TEST_NAME "sodium_core"
#include "cmptest.h"

int main(void)
{
  printf ("%d\n", sodium_init());

  return 0;
}
