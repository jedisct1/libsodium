#include <string>
using std::string;
#include "crypto_scalarmult.h"

string crypto_scalarmult_base(const string &n)
{
  unsigned char q[crypto_scalarmult_BYTES];
  if (n.size() != crypto_scalarmult_SCALARBYTES) throw "incorrect scalar length";
  crypto_scalarmult_base(q,(const unsigned char *) n.c_str());
  return string((char *) q,sizeof q);
}
