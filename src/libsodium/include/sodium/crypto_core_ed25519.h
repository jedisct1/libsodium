#ifndef crypto_core_ed25519_H
#define crypto_core_ed25519_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_core_ed25519_add(unsigned char *r,
                            const unsigned char *p, const unsigned char *q);

SODIUM_EXPORT
int crypto_core_ed25519_sub(unsigned char *r,
                            const unsigned char *p, const unsigned char *q);

#ifdef __cplusplus
}
#endif

#endif


