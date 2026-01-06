#ifndef turboshake128_ref_H
#define turboshake128_ref_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_core_keccak1600.h"

#define TURBOSHAKE128_RATE 168

typedef enum {
    TURBOSHAKE128_PHASE_ABSORBING = 0,
    TURBOSHAKE128_PHASE_SQUEEZING = 1
} turboshake128_phase;

typedef struct turboshake128_state_internal_ {
    crypto_core_keccak1600_state state;
    size_t                       offset;
    uint8_t                      phase;
    unsigned char                domain;
} turboshake128_state_internal;

int turboshake128_ref(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen);

int turboshake128_ref_init(turboshake128_state_internal *state);

int turboshake128_ref_init_with_domain(turboshake128_state_internal *state, unsigned char domain);

int turboshake128_ref_update(turboshake128_state_internal *state,
                             const unsigned char          *in,
                             size_t                        inlen);

int turboshake128_ref_squeeze(turboshake128_state_internal *state, unsigned char *out,
                              size_t outlen);

#endif /* turboshake128_ref_H */
