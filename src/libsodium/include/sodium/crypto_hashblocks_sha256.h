#ifndef crypto_hashblocks_sha256_H
#define crypto_hashblocks_sha256_H

#include "export.h"

#define crypto_hashblocks_sha256_STATEBYTES 32U
#define crypto_hashblocks_sha256_BLOCKBYTES 64U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_hashblocks_sha256(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#define crypto_hashblocks_sha256_ref crypto_hashblocks_sha256

#endif
