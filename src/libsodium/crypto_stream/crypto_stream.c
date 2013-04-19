
#include "crypto_stream.h"

int
crypto_stream(unsigned char *c, unsigned long long clen,
              const unsigned char *n, const unsigned char *k)
{
    return crypto_stream_xsalsa20(c, clen, n, k);
}


int
crypto_stream_xor(unsigned char *c, const unsigned char *m,
                  unsigned long long mlen, const unsigned char *n,
                  const unsigned char *k)
{
    return crypto_stream_xsalsa20_xor(c, m, mlen, n, k);
}
