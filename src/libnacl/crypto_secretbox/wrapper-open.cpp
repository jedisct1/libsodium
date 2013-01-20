#include <string>
using std::string;
#include "crypto_secretbox.h"

string crypto_secretbox_open(const string &c,const string &n,const string &k)
{
  if (k.size() != crypto_secretbox_KEYBYTES) throw "incorrect key length";
  if (n.size() != crypto_secretbox_NONCEBYTES) throw "incorrect nonce length";
  size_t clen = c.size() + crypto_secretbox_BOXZEROBYTES;
  unsigned char cpad[clen];
  for (int i = 0;i < crypto_secretbox_BOXZEROBYTES;++i) cpad[i] = 0;
  for (int i = crypto_secretbox_BOXZEROBYTES;i < clen;++i) cpad[i] = c[i - crypto_secretbox_BOXZEROBYTES];
  unsigned char mpad[clen];
  if (crypto_secretbox_open(mpad,cpad,clen,(const unsigned char *) n.c_str(),(const unsigned char *) k.c_str()) != 0)
    throw "ciphertext fails verification";
  if (clen < crypto_secretbox_ZEROBYTES)
    throw "ciphertext too short"; // should have been caught by _open
  return string(
    (char *) mpad + crypto_secretbox_ZEROBYTES,
    clen - crypto_secretbox_ZEROBYTES
  );
}
