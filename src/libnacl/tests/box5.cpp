#include <string>
using std::string;
#include <stdio.h>
#include "crypto_box.h"
#include "randombytes.h"

main()
{
  int mlen;
  for (mlen = 0;mlen < 1000;++mlen) {
    string alicesk;
    string alicepk = crypto_box_keypair(&alicesk);
    string bobsk;
    string bobpk = crypto_box_keypair(&bobsk);
    unsigned char nbytes[crypto_box_NONCEBYTES];
    randombytes(nbytes,crypto_box_NONCEBYTES);
    string n((char *) nbytes,crypto_box_NONCEBYTES);
    unsigned char mbytes[mlen];
    randombytes(mbytes,mlen);
    string m((char *) mbytes,mlen);
    string c = crypto_box(m,n,bobpk,alicesk);
    try {
      string m2 = crypto_box_open(c,n,alicepk,bobsk);
      if (m != m2) printf("bad decryption\n");
    } catch(const char *s) {
      printf("%s\n",s);
    }
  }
  return 0;
}
