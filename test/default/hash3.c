#include <stdio.h>

#define TEST_NAME "hash3"
#include "cmptest.h"

unsigned char x[] = "testing\n";
unsigned char h[crypto_hash_sha512_BYTES];

int main(void)
{
  size_t i;
  crypto_hash_sha512(h,x,sizeof x - 1U);
  for (i = 0;i < crypto_hash_sha512_BYTES;++i) printf("%02x",(unsigned int) h[i]);
  printf("\n");
  return 0;
}
