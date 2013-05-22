#ifndef crypto_verify_32_H
#define crypto_verify_32_H

#include <stddef.h>
#include "export.h"

#define crypto_verify_32_BYTES 32U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_verify_32_bytes(void);

SODIUM_EXPORT
int crypto_verify_32(const unsigned char *x, const unsigned char *y);

#define crypto_verify_32_ref crypto_verify_32

#ifdef __cplusplus
}
#endif

#endif
