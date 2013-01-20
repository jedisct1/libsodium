#include <string>
using std::string;
#include "crypto_secretbox.h"

string crypto_secretbox(const string &m,const string &n,const string &k)
{
  if (k.size() != crypto_secretbox_KEYBYTES) throw "incorrect key length";
  if (n.size() != crypto_secretbox_NONCEBYTES) throw "incorrect nonce length";
  size_t mlen = m.size() + crypto_secretbox_ZEROBYTES;
  unsigned char mpad[mlen];
  for (int i = 0;i < crypto_secretbox_ZEROBYTES;++i) mpad[i] = 0;
  for (int i = crypto_secretbox_ZEROBYTES;i < mlen;++i) mpad[i] = m[i - crypto_secretbox_ZEROBYTES];
  unsigned char cpad[mlen];
  crypto_secretbox(cpad,mpad,mlen,(const unsigned char *) n.c_str(),(const unsigned char *) k.c_str());
  return string(
    (char *) cpad + crypto_secretbox_BOXZEROBYTES,
    mlen - crypto_secretbox_BOXZEROBYTES
  );
}
