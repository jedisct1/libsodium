#ifndef crypto_stream_H
#define crypto_stream_H

/*
 *  WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 *  While it provides some protection against eavesdropping, it does NOT
 *  provide any security against active attacks.
 *  Unless you know what you're doing, what you are looking for is probably
 *  the crypto_box functions.
 */

#include <stddef.h>
#include <stdint.h>

#include "crypto_stream_xsalsa20.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_stream_KEYBYTES crypto_stream_xsalsa20_KEYBYTES
SODIUM_EXPORT
size_t  crypto_stream_keybytes(void);

#define crypto_stream_NONCEBYTES crypto_stream_xsalsa20_NONCEBYTES
SODIUM_EXPORT
size_t  crypto_stream_noncebytes(void);

#define crypto_stream_PRIMITIVE "xsalsa20"
SODIUM_EXPORT
const char *crypto_stream_primitive(void);

SODIUM_EXPORT
int crypto_stream(unsigned char *c, uint64_t clen,
                  const unsigned char *n, const unsigned char *k);

SODIUM_EXPORT
int crypto_stream_xor(unsigned char *c, const unsigned char *m,
                      uint64_t mlen, const unsigned char *n,
                      const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
