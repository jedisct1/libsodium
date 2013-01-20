#include <string>
using std::string;
#include "crypto_hash.h"

string crypto_hash(const string &m)
{
  unsigned char h[crypto_hash_BYTES];
  crypto_hash(h,(const unsigned char *) m.c_str(),m.size());
  return string((char *) h,sizeof h);
}
