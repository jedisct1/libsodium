#include <string>
using std::string;
#include <stdio.h>
#include <stdlib.h>
#include "crypto_auth_hmacsha512256.h"
#include "randombytes.h"

main()
{
  int clen;
  int i;
  for (clen = 0;clen < 10000;++clen) {
    unsigned char key_bytes[32];
    randombytes(key_bytes,sizeof key_bytes);
    string key((char *) key_bytes,sizeof key_bytes);
    unsigned char c_bytes[clen];
    randombytes(c_bytes,sizeof c_bytes);
    string c((char *) c_bytes,sizeof c_bytes);
    string a = crypto_auth_hmacsha512256(c,key);
    try {
      crypto_auth_hmacsha512256_verify(a,c,key);
    } catch(const char *s) {
      printf("fail %d %s\n",clen,s);
      return 100;
    }
    if (clen > 0) {
      size_t pos = random() % clen;
      c.replace(pos,1,1,c[pos] + 1 + (random() % 255));
      try {
        crypto_auth_hmacsha512256_verify(a,c,key);
	printf("forgery %d\n",clen);
      } catch(const char *s) {
        ;
      }
      pos = random() % a.size();
      a.replace(pos,1,1,a[pos] + 1 + (random() % 255));
      try {
        crypto_auth_hmacsha512256_verify(a,c,key);
	printf("forgery %d\n",clen);
      } catch(const char *s) {
        ;
      }
    }
  }
  return 0;
}
