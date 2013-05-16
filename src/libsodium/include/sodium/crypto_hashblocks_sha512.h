#ifndef crypto_hashblocks_sha512_H
#define crypto_hashblocks_sha512_H

#include <stddef.h>
#include "export.h"

#define crypto_hashblocks_sha512_STATEBYTES 64U
#define crypto_hashblocks_sha512_BLOCKBYTES 128U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_hashblocks_sha512_statebytes(void);

SODIUM_EXPORT
size_t crypto_hashblocks_sha512_blockbytes(void);

SODIUM_EXPORT
const char * crypto_hashblocks_sha512_primitive(void);

SODIUM_EXPORT
int crypto_hashblocks_sha512(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#define crypto_hashblocks_sha512_ref crypto_hashblocks_sha512

#endif
