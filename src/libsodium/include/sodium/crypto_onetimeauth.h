#ifndef crypto_onetimeauth_H
#define crypto_onetimeauth_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_onetimeauth_poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_onetimeauth_BYTES crypto_onetimeauth_poly1305_BYTES
SODIUM_EXPORT
size_t  crypto_onetimeauth_bytes(void);

#define crypto_onetimeauth_KEYBYTES crypto_onetimeauth_poly1305_KEYBYTES
SODIUM_EXPORT
size_t  crypto_onetimeauth_keybytes(void);

#define crypto_onetimeauth_PRIMITIVE "poly1305"
SODIUM_EXPORT
const char *crypto_onetimeauth_primitive(void);

SODIUM_EXPORT
int crypto_onetimeauth(unsigned char *out, const unsigned char *in,
                       uint64_t inlen, const unsigned char *k);

SODIUM_EXPORT
int crypto_onetimeauth_verify(const unsigned char *h, const unsigned char *in,
                              uint64_t inlen, const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
