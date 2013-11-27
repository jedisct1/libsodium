#ifndef crypto_hashblocks_sha256_H
#define crypto_hashblocks_sha256_H

#include <stddef.h>
#include <stdint.h>
#include "export.h"

#define crypto_hashblocks_sha256_STATEBYTES 32U
#define crypto_hashblocks_sha256_BLOCKBYTES 64U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_hashblocks_sha256_statebytes(void);

SODIUM_EXPORT
size_t crypto_hashblocks_sha256_blockbytes(void);

SODIUM_EXPORT
const char * crypto_hashblocks_sha256_primitive(void);

SODIUM_EXPORT
int crypto_hashblocks_sha256(unsigned char *,const unsigned char *,uint64_t);

#ifdef __cplusplus
}
#endif

#define crypto_hashblocks_sha256_ref crypto_hashblocks_sha256

#endif
