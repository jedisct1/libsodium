#ifndef crypto_hashblocks_sha512_H
#define crypto_hashblocks_sha512_H

#include "export.h"

#define crypto_hashblocks_sha512_STATEBYTES 64
#define crypto_hashblocks_sha512_BLOCKBYTES 128

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_hashblocks_sha512(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#define crypto_hashblocks_sha512_ref crypto_hashblocks_sha512

#endif
