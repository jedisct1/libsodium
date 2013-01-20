#include <string>
using std::string;
#include "crypto_onetimeauth.h"

string crypto_onetimeauth(const string &m,const string &k)
{
  if (k.size() != crypto_onetimeauth_KEYBYTES) throw "incorrect key length";
  unsigned char a[crypto_onetimeauth_BYTES];
  crypto_onetimeauth(a,(const unsigned char *) m.c_str(),m.size(),(const unsigned char *) k.c_str());
  return string((char *) a,crypto_onetimeauth_BYTES);
}
