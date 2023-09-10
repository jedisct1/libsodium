#ifndef aegis128l_implementations_H
#define aegis128l_implementations_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_aead_aegis128l.h"

typedef struct aegis128l_implementation {
    int (*encrypt_detached)(uint8_t *c, uint8_t *mac, size_t maclen, const uint8_t *m, size_t mlen,
                            const uint8_t *ad, size_t adlen, const uint8_t *npub, const uint8_t *k);
    int (*decrypt_detached)(uint8_t *m, const uint8_t *c, size_t clen, const uint8_t *mac,
                            size_t maclen, const uint8_t *ad, size_t adlen, const uint8_t *npub,
                            const uint8_t *k);
} aegis128l_implementation;

#endif
