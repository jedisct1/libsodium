#include <string>
using std::string;
#include "crypto_scalarmult.h"

string crypto_scalarmult(const string &n,const string &p)
{
  unsigned char q[crypto_scalarmult_BYTES];
  if (n.size() != crypto_scalarmult_SCALARBYTES) throw "incorrect scalar length";
  if (p.size() != crypto_scalarmult_BYTES) throw "incorrect element length";
  crypto_scalarmult(q,(const unsigned char *) n.c_str(),(const unsigned char *) p.c_str());
  return string((char *) q,sizeof q);
}
