#include <string>
using std::string;
#include "crypto_box.h"

string crypto_box(const string &m,const string &n,const string &pk,const string &sk)
{
  if (pk.size() != crypto_box_PUBLICKEYBYTES) throw "incorrect public-key length";
  if (sk.size() != crypto_box_SECRETKEYBYTES) throw "incorrect secret-key length";
  if (n.size() != crypto_box_NONCEBYTES) throw "incorrect nonce length";
  size_t mlen = m.size() + crypto_box_ZEROBYTES;
  unsigned char mpad[mlen];
  for (int i = 0;i < crypto_box_ZEROBYTES;++i) mpad[i] = 0;
  for (int i = crypto_box_ZEROBYTES;i < mlen;++i) mpad[i] = m[i - crypto_box_ZEROBYTES];
  unsigned char cpad[mlen];
  crypto_box(cpad,mpad,mlen,
    (const unsigned char *) n.c_str(),
    (const unsigned char *) pk.c_str(),
    (const unsigned char *) sk.c_str()
    );
  return string(
    (char *) cpad + crypto_box_BOXZEROBYTES,
    mlen - crypto_box_BOXZEROBYTES
  );
}
