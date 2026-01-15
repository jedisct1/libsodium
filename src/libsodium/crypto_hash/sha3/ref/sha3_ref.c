#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "crypto_hash_sha3.h"
#include "private/common.h"
#include "sha3_ref.h"
#include "utils.h"

static int
sha3_ref_init(sha3_state_internal *state, size_t rate, size_t outlen)
{
    crypto_core_keccak1600_init(&state->state);
    state->offset = 0;
    state->rate   = rate;
    state->outlen = outlen;
    state->phase  = SHA3_PHASE_ABSORBING;

    return 0;
}

static int
sha3_ref_update(sha3_state_internal *state, const unsigned char *in, size_t inlen)
{
    size_t consumed = 0;
    size_t chunk_size;

    while (consumed < inlen) {
        if (state->offset == state->rate) {
            crypto_core_keccak1600_permute_24(&state->state);
            state->offset = 0;
        }
        chunk_size = state->rate - state->offset;
        if (chunk_size > inlen - consumed) {
            chunk_size = inlen - consumed;
        }
        crypto_core_keccak1600_xor_bytes(&state->state, &in[consumed], state->offset, chunk_size);
        state->offset += chunk_size;
        consumed += chunk_size;
    }

    return 0;
}

static int
sha3_ref_final(sha3_state_internal *state, unsigned char *out)
{
    unsigned char pad;

    if (state->offset == state->rate) {
        crypto_core_keccak1600_permute_24(&state->state);
        state->offset = 0;
    }

    if (state->offset == state->rate - 1) {
        pad = (unsigned char) (SHA3_DOMAIN ^ 0x80);
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, state->offset, 1);
    } else {
        pad = SHA3_DOMAIN;
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, state->offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(&state->state, &pad, state->rate - 1, 1);
    }

    crypto_core_keccak1600_permute_24(&state->state);

    crypto_core_keccak1600_extract_bytes(&state->state, out, 0, state->outlen);

    sodium_memzero(state, sizeof *state);

    return 0;
}

int
sha3256_ref_init(sha3_state_internal *state)
{
    return sha3_ref_init(state, SHA3_256_RATE, crypto_hash_sha3256_BYTES);
}

int
sha3256_ref_update(sha3_state_internal *state, const unsigned char *in, size_t inlen)
{
    return sha3_ref_update(state, in, inlen);
}

int
sha3256_ref_final(sha3_state_internal *state, unsigned char *out)
{
    return sha3_ref_final(state, out);
}

int
sha3256_ref(unsigned char *out, const unsigned char *in, size_t inlen)
{
    sha3_state_internal state;

    sha3256_ref_init(&state);
    sha3256_ref_update(&state, in, inlen);
    sha3256_ref_final(&state, out);

    return 0;
}

int
sha3512_ref_init(sha3_state_internal *state)
{
    return sha3_ref_init(state, SHA3_512_RATE, crypto_hash_sha3512_BYTES);
}

int
sha3512_ref_update(sha3_state_internal *state, const unsigned char *in, size_t inlen)
{
    return sha3_ref_update(state, in, inlen);
}

int
sha3512_ref_final(sha3_state_internal *state, unsigned char *out)
{
    return sha3_ref_final(state, out);
}

int
sha3512_ref(unsigned char *out, const unsigned char *in, size_t inlen)
{
    sha3_state_internal state;

    sha3512_ref_init(&state);
    sha3512_ref_update(&state, in, inlen);
    sha3512_ref_final(&state, out);

    return 0;
}
