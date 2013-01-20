#include <string>
using std::string;
#include "crypto_stream.h"

string crypto_stream_xor(const string &m,const string &n,const string &k)
{
  if (n.size() != crypto_stream_NONCEBYTES) throw "incorrect nonce length";
  if (k.size() != crypto_stream_KEYBYTES) throw "incorrect key length";
  size_t mlen = m.size();
  unsigned char c[mlen];
  crypto_stream_xor(c,
    (const unsigned char *) m.c_str(),mlen,
    (const unsigned char *) n.c_str(),
    (const unsigned char *) k.c_str()
    );
  return string((char *) c,mlen);
}
