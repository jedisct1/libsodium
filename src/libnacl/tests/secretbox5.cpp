#include <string>
using std::string;
#include <stdio.h>
#include "crypto_secretbox.h"
#include "randombytes.h"

main()
{
  int mlen;
  for (mlen = 0;mlen < 1000;++mlen) {
    unsigned char kbytes[crypto_secretbox_KEYBYTES];
    randombytes(kbytes,crypto_secretbox_KEYBYTES);
    string k((char *) kbytes,crypto_secretbox_KEYBYTES);
    unsigned char nbytes[crypto_secretbox_NONCEBYTES];
    randombytes(nbytes,crypto_secretbox_NONCEBYTES);
    string n((char *) nbytes,crypto_secretbox_NONCEBYTES);
    unsigned char mbytes[mlen];
    randombytes(mbytes,mlen);
    string m((char *) mbytes,mlen);
    string c = crypto_secretbox(m,n,k);
    try {
      string m2 = crypto_secretbox_open(c,n,k);
      if (m != m2) printf("bad decryption\n");
    } catch(const char *s) {
      printf("%s\n",s);
    }
  }
  return 0;
}
