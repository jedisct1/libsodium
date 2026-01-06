#ifndef shake256_ref_H
#define shake256_ref_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_core_keccak1600.h"

#define SHAKE256_RATE 136

typedef enum { SHAKE256_PHASE_ABSORBING = 0, SHAKE256_PHASE_SQUEEZING = 1 } shake256_phase;

typedef struct shake256_state_internal_ {
    crypto_core_keccak1600_state state;
    size_t                       offset;
    uint8_t                      phase;
    unsigned char                domain;
} shake256_state_internal;

int shake256_ref(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen);

int shake256_ref_init(shake256_state_internal *state);

int shake256_ref_init_with_domain(shake256_state_internal *state, unsigned char domain);

int shake256_ref_update(shake256_state_internal *state, const unsigned char *in, size_t inlen);

int shake256_ref_squeeze(shake256_state_internal *state, unsigned char *out, size_t outlen);

#endif /* shake256_ref_H */
