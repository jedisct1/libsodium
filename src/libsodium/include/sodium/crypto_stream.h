#ifndef crypto_stream_H
#define crypto_stream_H

#include <stdlib.h>

#include "crypto_stream_xsalsa20.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_stream_KEYBYTES crypto_stream_xsalsa20_KEYBYTES
size_t  crypto_stream_keybytes(void);

#define crypto_stream_NONCEBYTES crypto_stream_xsalsa20_NONCEBYTES
size_t  crypto_stream_noncebytes(void);

#define crypto_stream_PRIMITIVE "xsalsa20"
const char *crypto_stream_primitive(void);

int crypto_stream(unsigned char *c, unsigned long long clen,
                  const unsigned char *n, const unsigned char *k);

int crypto_stream_xor(unsigned char *c, const unsigned char *m,
                      unsigned long long mlen, const unsigned char *n,
                      const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
