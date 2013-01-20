#include <string>
using std::string;
#include "crypto_auth.h"

string crypto_auth(const string &m,const string &k)
{
  if (k.size() != crypto_auth_KEYBYTES) throw "incorrect key length";
  unsigned char a[crypto_auth_BYTES];
  crypto_auth(a,(const unsigned char *) m.c_str(),m.size(),(const unsigned char *) k.c_str());
  return string((char *) a,crypto_auth_BYTES);
}
