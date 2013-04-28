#include <stdio.h>
#include <stdlib.h>
#include "windows/windows-quirks.h"

#define TEST_NAME "auth5"
#include "cmptest.h"

unsigned char key[32];
unsigned char c[10000];
unsigned char a[32];

int main(void)
{
  int clen;
  for (clen = 0;clen < 10000;++clen) {
    randombytes(key,sizeof key);
    randombytes(c,clen);
    crypto_auth_hmacsha512256(a,c,clen,key);
    if (crypto_auth_hmacsha512256_verify(a,c,clen,key) != 0) {
      printf("fail %d\n",clen);
      return 100;
    }
    if (clen > 0) {
      c[rand() % clen] += 1 + (rand() % 255);
      if (crypto_auth_hmacsha512256_verify(a,c,clen,key) == 0) {
        printf("forgery %d\n",clen);
        return 100;
      }
      a[rand() % sizeof a] += 1 + (rand() % 255);
      if (crypto_auth_hmacsha512256_verify(a,c,clen,key) == 0) {
        printf("forgery %d\n",clen);
        return 100;
      }
    }
  }
  return 0;
}
