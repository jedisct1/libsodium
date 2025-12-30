#ifndef crypto_core_keccak1600_H
#define crypto_core_keccak1600_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_core_keccak1600_STATEBYTES 200U
SODIUM_EXPORT
size_t crypto_core_keccak1600_statebytes(void);

/* Initialize state to all zeros */
SODIUM_EXPORT
void crypto_core_keccak1600_init(void *state)
            __attribute__ ((nonnull));

/* XOR bytes into state (for absorbing) */
SODIUM_EXPORT
void crypto_core_keccak1600_xor_bytes(void *state, const unsigned char *bytes,
                                      size_t offset, size_t length)
            __attribute__ ((nonnull));

/* Extract bytes from state (for squeezing) */
SODIUM_EXPORT
void crypto_core_keccak1600_extract_bytes(const void *state, unsigned char *bytes,
                                          size_t offset, size_t length)
            __attribute__ ((nonnull));

/* Keccak-f[1600]: 24 rounds (for SHAKE) */
SODIUM_EXPORT
void crypto_core_keccak1600_permute_24(void *state)
            __attribute__ ((nonnull));

/* Keccak-p[1600,12]: 12 rounds (for TurboSHAKE) */
SODIUM_EXPORT
void crypto_core_keccak1600_permute_12(void *state)
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
