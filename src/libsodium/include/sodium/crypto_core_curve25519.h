#ifndef crypto_core_curve25519_H
#define crypto_core_curve25519_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_core_curve25519_is_valid_point(const unsigned char *p);

#ifdef __cplusplus
}
#endif

#endif
