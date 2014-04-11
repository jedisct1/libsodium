#ifndef _crypto_hashblocks_sha512_H
#define _crypto_hashblocks_sha512_H

#include <stddef.h>
#include "export.h"

#define crypto_hashblocks_sha512_STATEBYTES 64U
#define crypto_hashblocks_sha512_BLOCKBYTES 128U

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

/* This function is only used internally */
int crypto_hashblocks_sha512(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#endif
