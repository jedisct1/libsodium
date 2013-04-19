#ifndef crypto_stream_H
#define crypto_stream_H

#include "crypto_stream_xsalsa20.h"

#define crypto_stream_KEYBYTES crypto_stream_xsalsa20_KEYBYTES
#define crypto_stream_NONCEBYTES crypto_stream_xsalsa20_NONCEBYTES
#define crypto_stream_PRIMITIVE "xsalsa20"

int crypto_stream(unsigned char *c, unsigned long long clen,
                  const unsigned char *n, const unsigned char *k);


int crypto_stream_xor(unsigned char *c, const unsigned char *m,
                      unsigned long long mlen, const unsigned char *n,
                      const unsigned char *k);

#endif
