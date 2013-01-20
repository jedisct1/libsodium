#include <stdio.h>
#include <stdlib.h>
#include "crypto_auth_hmacsha512256.h"
#include "randombytes.h"
#include "windows/windows-quirks.h"

unsigned char key[32];
unsigned char c[10000];
unsigned char a[32];

main()
{
  int clen;
  int i;
  for (clen = 0;clen < 10000;++clen) {
    randombytes(key,sizeof key);
    randombytes(c,clen);
    crypto_auth_hmacsha512256(a,c,clen,key);
    if (crypto_auth_hmacsha512256_verify(a,c,clen,key) != 0) {
      printf("fail %d\n",clen);
      return 100;
    }
    if (clen > 0) {
      c[random() % clen] += 1 + (random() % 255);
      if (crypto_auth_hmacsha512256_verify(a,c,clen,key) == 0) {
        printf("forgery %d\n",clen);
        return 100;
      }
      a[random() % sizeof a] += 1 + (random() % 255);
      if (crypto_auth_hmacsha512256_verify(a,c,clen,key) == 0) {
        printf("forgery %d\n",clen);
        return 100;
      }
    }
  }
  return 0;
}
