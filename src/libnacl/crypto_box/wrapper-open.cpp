#include <string>
using std::string;
#include "crypto_box.h"

string crypto_box_open(const string &c,const string &n,const string &pk,const string &sk)
{
  if (pk.size() != crypto_box_PUBLICKEYBYTES) throw "incorrect public-key length";
  if (sk.size() != crypto_box_SECRETKEYBYTES) throw "incorrect secret-key length";
  if (n.size() != crypto_box_NONCEBYTES) throw "incorrect nonce length";
  size_t clen = c.size() + crypto_box_BOXZEROBYTES;
  unsigned char cpad[clen];
  for (int i = 0;i < crypto_box_BOXZEROBYTES;++i) cpad[i] = 0;
  for (int i = crypto_box_BOXZEROBYTES;i < clen;++i) cpad[i] = c[i - crypto_box_BOXZEROBYTES];
  unsigned char mpad[clen];
  if (crypto_box_open(mpad,cpad,clen,
                       (const unsigned char *) n.c_str(),
                       (const unsigned char *) pk.c_str(),
                       (const unsigned char *) sk.c_str()
                     ) != 0)
    throw "ciphertext fails verification";
  if (clen < crypto_box_ZEROBYTES)
    throw "ciphertext too short"; // should have been caught by _open
  return string(
    (char *) mpad + crypto_box_ZEROBYTES,
    clen - crypto_box_ZEROBYTES
  );
}
