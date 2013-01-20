#include <stdio.h>
#include "crypto_hash_sha512.h"

unsigned char x[8] = "testing\n";
unsigned char h[crypto_hash_sha512_BYTES];

int main()
{
  int i;
  crypto_hash_sha512(h,x,sizeof x);
  for (i = 0;i < crypto_hash_sha512_BYTES;++i) printf("%02x",(unsigned int) h[i]);
  printf("\n");
  return 0;
}
