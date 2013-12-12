#ifndef crypto_scalarmult_curve25519_H
#define crypto_scalarmult_curve25519_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_scalarmult_curve25519_BYTES 32
SODIUM_EXPORT
size_t crypto_scalarmult_curve25519_bytes(void);

#define crypto_scalarmult_curve25519_SCALARBYTES 32
SODIUM_EXPORT
size_t crypto_scalarmult_curve25519_scalarbytes(void);

SODIUM_EXPORT
int crypto_scalarmult_curve25519(unsigned char *,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_scalarmult_curve25519_base(unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#endif
