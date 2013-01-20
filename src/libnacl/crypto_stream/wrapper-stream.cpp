#include <string>
using std::string;
#include "crypto_stream.h"

string crypto_stream(size_t clen,const string &n,const string &k)
{
  if (n.size() != crypto_stream_NONCEBYTES) throw "incorrect nonce length";
  if (k.size() != crypto_stream_KEYBYTES) throw "incorrect key length";
  unsigned char c[clen];
  crypto_stream(c,clen,(const unsigned char *) n.c_str(),(const unsigned char *) k.c_str());
  return string((char *) c,clen);
}
