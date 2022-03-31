#ifndef crypto_zkp_shifumi128_H
#define crypto_zkp_shifumi128_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum crypto_zkp_shifumi128_commitment {
    crypto_zkp_shifumi128_ROCK,
    crypto_zkp_shifumi128_PAPER,
    crypto_zkp_shifumi128_SCISSORS
} crypto_zkp_shifumi128_commitment;

#define crypto_zkp_shifumi128_SECRETBYTES 16
#define crypto_zkp_shifumi128_BYTES       16

SODIUM_EXPORT
size_t crypto_zkp_shifumi128_secretbytes(void);

SODIUM_EXPORT
size_t crypto_zkp_shifumi128_bytes(void);

SODIUM_EXPORT
void crypto_zkp_shufumi128_pick(crypto_zkp_shifumi128_commitment *commitment);

SODIUM_EXPORT
int crypto_zkp_shifumi128_commit(unsigned char secret[crypto_zkp_shifumi128_SECRETBYTES],
                                 unsigned char proof[crypto_zkp_shifumi128_BYTES],
                                 const crypto_zkp_shifumi128_commitment commitment);

SODIUM_EXPORT
int crypto_zkp_shifumi128_verify(const unsigned char secret[crypto_zkp_shifumi128_SECRETBYTES],
                                 const unsigned char proof[crypto_zkp_shifumi128_BYTES],
                                 const crypto_zkp_shifumi128_commitment commitment);

#ifdef __cplusplus
}
#endif

#endif
