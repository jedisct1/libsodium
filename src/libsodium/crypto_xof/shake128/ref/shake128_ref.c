#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "crypto_xof_shake128.h"
#include "private/common.h"
#include "shake128_ref.h"

int
shake128_ref_init_with_domain(shake128_state_internal *state, unsigned char domain)
{
    crypto_core_keccak1600_init(&state->state);
    state->offset = 0;
    state->phase  = SHAKE128_PHASE_ABSORBING;
    state->domain = domain;

    return 0;
}

int
shake128_ref_init(shake128_state_internal *state)
{
    return shake128_ref_init_with_domain(state, crypto_xof_shake128_DOMAIN_STANDARD);
}

int
shake128_ref_update(shake128_state_internal *state, const unsigned char *in, size_t inlen)
{
    size_t consumed = 0;
    size_t chunk_size;

    if (state->phase != SHAKE128_PHASE_ABSORBING) {
        state->phase  = SHAKE128_PHASE_ABSORBING;
        state->offset = 0;
    }

    while (consumed < inlen) {
        if (state->offset == SHAKE128_RATE) {
            crypto_core_keccak1600_permute_24(&state->state);
            state->offset = 0;
        }
        chunk_size = SHAKE128_RATE - state->offset;
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
shake128_finalize(shake128_state_internal *state)
{
    unsigned char pad;

    /* If the rate is exactly full, process that block before padding */
    if (state->offset == SHAKE128_RATE) {
        crypto_core_keccak1600_permute_24(&state->state);
        state->offset = 0;
    }

    /* Apply padding: domain byte at current position, 0x80 at last byte */
    if (state->offset == SHAKE128_RATE - 1) {
        /* Special case: padding fits in one byte */
        pad = (unsigned char) (state->domain ^ 0x80);
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, state->offset, 1);
    } else {
        /* Normal case: domain and 0x80 at different positions */
        crypto_core_keccak1600_xor_bytes(&state->state, &state->domain, state->offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, SHAKE128_RATE - 1, 1);
    }

    /* Final permutation */
    crypto_core_keccak1600_permute_24(&state->state);

    state->offset = 0;
    state->phase  = SHAKE128_PHASE_SQUEEZING;
}

int
shake128_ref_squeeze(shake128_state_internal *state, unsigned char *out, size_t outlen)
{
    size_t extracted = 0;
    size_t chunk_size;

    if (state->phase == SHAKE128_PHASE_ABSORBING) {
        shake128_finalize(state);
    }

    while (extracted < outlen) {
        if (state->offset == SHAKE128_RATE) {
            crypto_core_keccak1600_permute_24(&state->state);
            state->offset = 0;
        }
        chunk_size = SHAKE128_RATE - state->offset;
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
shake128_ref(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen)
{
    shake128_state_internal state;

    shake128_ref_init(&state);
    shake128_ref_update(&state, in, inlen);
    shake128_ref_squeeze(&state, out, outlen);

    return 0;
}
