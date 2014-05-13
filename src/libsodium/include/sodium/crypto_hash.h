#ifndef crypto_hash_H
#define crypto_hash_H

#include <stddef.h>

#include "crypto_hash_sha512.h"
#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_hash_BYTES crypto_hash_sha512_BYTES
SODIUM_EXPORT
size_t crypto_hash_bytes(void);

SODIUM_EXPORT
int crypto_hash(unsigned char *out, const unsigned char *in,
                unsigned long long inlen);

#define crypto_hash_PRIMITIVE "sha512"
SODIUM_EXPORT
const char *crypto_hash_primitive(void);

#ifdef __cplusplus
}
#endif

#endif
