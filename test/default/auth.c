
#define TEST_NAME "auth"
#include "cmptest.h"

/* "Test Case 2" from RFC 4231 */
unsigned char key[32] = "Jefe";
unsigned char c[] = "what do ya want for nothing?";

unsigned char a[32];

int main(void)
{
  int i;
  crypto_auth(a,c,sizeof c - 1U,key);
  for (i = 0;i < 32;++i) {
    printf(",0x%02x",(unsigned int) a[i]);
    if (i % 8 == 7) printf("\n");
  }

  assert(crypto_auth_bytes() > 0U);
  assert(crypto_auth_keybytes() > 0U);
  assert(strcmp(crypto_auth_primitive(), "hmacsha512256") == 0);

  return 0;
}
