#ifndef crypto_verify_64_H
#define crypto_verify_64_H

#include <stddef.h>
#include "export.h"

#define crypto_verify_64_BYTES 64U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_verify_64_bytes(void);

SODIUM_EXPORT
int crypto_verify_64(const unsigned char *x, const unsigned char *y);

#define crypto_verify_64_ref crypto_verify_64

#ifdef __cplusplus
}
#endif

#endif
