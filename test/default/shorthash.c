#include <stdio.h>
#include "crypto_shorthash.h"

#define TEST_NAME "shorthash"
#include "cmptest.h"

unsigned char x[8] = "testing\n";
unsigned char h[crypto_shorthash_BYTES];

int main(void)
{
  int i;
  crypto_shorthash(h,x,sizeof x);
  for (i = 0;i < crypto_shorthash_BYTES;++i) printf("%02x",(unsigned int) h[i]);
  printf("\n");
  return 0;
}
