#ifndef crypto_hash_H
#define crypto_hash_H

#include "crypto_hash_sha512.h"
#include "export.h"

#define crypto_hash_BYTES crypto_hash_sha512_BYTES
#define crypto_hash_BLOCKBYTES crypto_hash_sha512_BLOCKBYTES
#define crypto_hash_PRIMITIVE "sha512"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_hash(unsigned char *out, const unsigned char *in,
                unsigned long long inlen);

#ifdef __cplusplus
}
#endif

#endif
