#ifndef crypto_scalarmult_H
#define crypto_scalarmult_H

#include "crypto_scalarmult_curve25519.h"

#define crypto_scalarmult_BYTES crypto_scalarmult_curve25519_BYTES
#define crypto_scalarmult_SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES
#define crypto_scalarmult_PRIMITIVE "curve25519"

int crypto_scalarmult_base(unsigned char *q, const unsigned char *n);

int crypto_scalarmult(unsigned char *q, const unsigned char *n,
                      const unsigned char *p);

#endif
