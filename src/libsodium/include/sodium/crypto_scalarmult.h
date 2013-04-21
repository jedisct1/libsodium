#ifndef crypto_scalarmult_H
#define crypto_scalarmult_H

#include <stdlib.h>

#include "crypto_scalarmult_curve25519.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_scalarmult_BYTES crypto_scalarmult_curve25519_BYTES
size_t  crypto_scalarmult_bytes(void);

#define crypto_scalarmult_SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES
size_t  crypto_scalarmult_scalarbytes(void);

#define crypto_scalarmult_PRIMITIVE "curve25519"
const char *crypto_scalarmult_primitive(void);

int crypto_scalarmult_base(unsigned char *q, const unsigned char *n);

int crypto_scalarmult(unsigned char *q, const unsigned char *n,
                      const unsigned char *p);

#ifdef __cplusplus
}
#endif

#endif
