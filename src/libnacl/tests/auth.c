#include <stdio.h>
#include "crypto_auth_hmacsha512256.h"

/* "Test Case 2" from RFC 4231 */
unsigned char key[32] = "Jefe";
unsigned char c[28] = "what do ya want for nothing?";

unsigned char a[32];

main()
{
  int i;
  crypto_auth_hmacsha512256(a,c,sizeof c,key);
  for (i = 0;i < 32;++i) {
    printf(",0x%02x",(unsigned int) a[i]);
    if (i % 8 == 7) printf("\n");
  }
  return 0;
}
