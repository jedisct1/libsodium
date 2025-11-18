#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "private/common.h"
#include "turboshake256_ref.h"

#define TURBOSHAKE256_DOMAIN_BYTE_STANDARD 0x01

int
turboshake256_ref_init_with_domain(turboshake256_state_internal *state, unsigned char domain)
{
    crypto_core_keccak1600_init(state->state);
    state->offset = 0;
    state->phase  = TURBOSHAKE256_PHASE_ABSORBING;
    state->domain = domain;

    return 0;
}

int
turboshake256_ref_init(turboshake256_state_internal *state)
{
    return turboshake256_ref_init_with_domain(state, TURBOSHAKE256_DOMAIN_BYTE_STANDARD);
}

int
turboshake256_ref_update(turboshake256_state_internal *state,
                         const unsigned char          *in,
                         unsigned long long            inlen)
{
    size_t i;

    if (state->phase != TURBOSHAKE256_PHASE_ABSORBING) {
        state->phase = TURBOSHAKE256_PHASE_ABSORBING;
        state->offset = 0;
    }

    for (i = 0; i < inlen; i++) {
        if (state->offset == TURBOSHAKE256_RATE) {
            crypto_core_keccak1600_permute_12(state->state);
            state->offset = 0;
        }
        crypto_core_keccak1600_xor_bytes(state->state, &in[i], state->offset, 1);
        state->offset++;
    }

    return 0;
}

static void
turboshake256_finalize(turboshake256_state_internal *state)
{
    unsigned char pad;

    /* Apply padding: domain byte at current position, 0x80 at last byte */
    if (state->offset == TURBOSHAKE256_RATE - 1) {
        /* Special case: padding fits in one byte */
        pad = state->domain | 0x80;
        crypto_core_keccak1600_xor_bytes(state->state, &pad, state->offset, 1);
    } else {
        /* Normal case: domain and 0x80 at different positions */
        crypto_core_keccak1600_xor_bytes(state->state, &state->domain, state->offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(state->state, &pad, TURBOSHAKE256_RATE - 1, 1);
    }

    /* Final permutation (12 rounds for TurboSHAKE) */
    crypto_core_keccak1600_permute_12(state->state);

    state->offset = 0;
    state->phase  = TURBOSHAKE256_PHASE_SQUEEZING;
}

int
turboshake256_ref_squeeze(turboshake256_state_internal *state, unsigned char *out, size_t outlen)
{
    size_t i;

    if (state->phase == TURBOSHAKE256_PHASE_ABSORBING) {
        turboshake256_finalize(state);
    }

    for (i = 0; i < outlen; i++) {
        if (state->offset == TURBOSHAKE256_RATE) {
            crypto_core_keccak1600_permute_12(state->state);
            state->offset = 0;
        }
        crypto_core_keccak1600_extract_bytes(state->state, &out[i], state->offset, 1);
        state->offset++;
    }

    return 0;
}

int
turboshake256_ref(unsigned char *out, size_t outlen, const unsigned char *in,
                  unsigned long long inlen)
{
    turboshake256_state_internal state;

    turboshake256_ref_init(&state);
    turboshake256_ref_update(&state, in, inlen);
    turboshake256_ref_squeeze(&state, out, outlen);

    return 0;
}
