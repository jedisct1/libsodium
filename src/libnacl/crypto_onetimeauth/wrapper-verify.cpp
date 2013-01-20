#include <string>
using std::string;
#include "crypto_onetimeauth.h"

void crypto_onetimeauth_verify(const string &a,const string &m,const string &k)
{
  if (k.size() != crypto_onetimeauth_KEYBYTES) throw "incorrect key length";
  if (a.size() != crypto_onetimeauth_BYTES) throw "incorrect authenticator length";
  if (crypto_onetimeauth_verify(
       (const unsigned char *) a.c_str(),
       (const unsigned char *) m.c_str(),m.size(),
       (const unsigned char *) k.c_str()) == 0) return;
  throw "invalid authenticator";
}
