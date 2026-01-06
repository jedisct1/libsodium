#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "crypto_xof_turboshake256.h"
#include "private/common.h"
#include "turboshake256_ref.h"

int
turboshake256_ref_init_with_domain(turboshake256_state_internal *state, unsigned char domain)
{
    crypto_core_keccak1600_init(&state->state);
    state->offset = 0;
    state->phase  = TURBOSHAKE256_PHASE_ABSORBING;
    state->domain = domain;

    return 0;
}

int
turboshake256_ref_init(turboshake256_state_internal *state)
{
    return turboshake256_ref_init_with_domain(state, crypto_xof_turboshake256_DOMAIN_STANDARD);
}

int
turboshake256_ref_update(turboshake256_state_internal *state, const unsigned char *in, size_t inlen)
{
    size_t consumed = 0;
    size_t chunk_size;

    if (state->phase != TURBOSHAKE256_PHASE_ABSORBING) {
        state->phase  = TURBOSHAKE256_PHASE_ABSORBING;
        state->offset = 0;
    }

    while (consumed < inlen) {
        if (state->offset == TURBOSHAKE256_RATE) {
            crypto_core_keccak1600_permute_12(&state->state);
            state->offset = 0;
        }
        chunk_size = TURBOSHAKE256_RATE - state->offset;
        if (chunk_size > inlen - consumed) {
            chunk_size = inlen - consumed;
        }
        crypto_core_keccak1600_xor_bytes(&state->state, &in[consumed], state->offset, chunk_size);
        state->offset += chunk_size;
        consumed += chunk_size;
    }

    return 0;
}

static void
turboshake256_finalize(turboshake256_state_internal *state)
{
    unsigned char pad;

    /* If the rate is exactly full, process that block before padding */
    if (state->offset == TURBOSHAKE256_RATE) {
        crypto_core_keccak1600_permute_12(&state->state);
        state->offset = 0;
    }

    /* Apply padding: domain byte at current position, 0x80 at last byte */
    if (state->offset == TURBOSHAKE256_RATE - 1) {
        /* Special case: padding fits in one byte */
        pad = (unsigned char) (state->domain ^ 0x80);
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, state->offset, 1);
    } else {
        /* Normal case: domain and 0x80 at different positions */
        crypto_core_keccak1600_xor_bytes(&state->state, &state->domain, state->offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, TURBOSHAKE256_RATE - 1, 1);
    }

    /* Final permutation (12 rounds for TurboSHAKE) */
    crypto_core_keccak1600_permute_12(&state->state);

    state->offset = 0;
    state->phase  = TURBOSHAKE256_PHASE_SQUEEZING;
}

int
turboshake256_ref_squeeze(turboshake256_state_internal *state, unsigned char *out, size_t outlen)
{
    size_t extracted = 0;
    size_t chunk_size;

    if (state->phase == TURBOSHAKE256_PHASE_ABSORBING) {
        turboshake256_finalize(state);
    }

    while (extracted < outlen) {
        if (state->offset == TURBOSHAKE256_RATE) {
            crypto_core_keccak1600_permute_12(&state->state);
            state->offset = 0;
        }
        chunk_size = TURBOSHAKE256_RATE - state->offset;
        if (chunk_size > outlen - extracted) {
            chunk_size = outlen - extracted;
        }
        crypto_core_keccak1600_extract_bytes(&state->state, &out[extracted], state->offset,
                                             chunk_size);
        state->offset += chunk_size;
        extracted += chunk_size;
    }

    return 0;
}

int
turboshake256_ref(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen)
{
    turboshake256_state_internal state;

    turboshake256_ref_init(&state);
    turboshake256_ref_update(&state, in, inlen);
    turboshake256_ref_squeeze(&state, out, outlen);

    return 0;
}
