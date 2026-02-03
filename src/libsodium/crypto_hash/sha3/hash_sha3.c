#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "crypto_hash_sha3.h"
#include "private/common.h"
#include "utils.h"

#define SHA3_256_RATE 136
#define SHA3_512_RATE 72
#define SHA3_DOMAIN   0x06

typedef enum { SHA3_PHASE_ABSORBING = 0, SHA3_PHASE_FINALIZED = 1 } sha3_phase;

typedef struct sha3_state_internal_ {
    crypto_core_keccak1600_state state;
    size_t                       offset;
    size_t                       rate;
    size_t                       outlen;
    uint8_t                      phase;
} sha3_state_internal;

static int
sha3_init(sha3_state_internal *state, size_t rate, size_t outlen)
{
    crypto_core_keccak1600_init(&state->state);
    state->offset = 0;
    state->rate   = rate;
    state->outlen = outlen;
    state->phase  = SHA3_PHASE_ABSORBING;

    return 0;
}

static int
sha3_update(sha3_state_internal *state, const unsigned char *in, size_t inlen)
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
sha3_final(sha3_state_internal *state, unsigned char *out)
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

size_t
crypto_hash_sha3256_bytes(void)
{
    return crypto_hash_sha3256_BYTES;
}

size_t
crypto_hash_sha3256_statebytes(void)
{
    return sizeof(crypto_hash_sha3256_state);
}

int
crypto_hash_sha3256_init(crypto_hash_sha3256_state *state)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3256_state) >= sizeof(sha3_state_internal));
    return sha3_init((sha3_state_internal *) (void *) state,
                     SHA3_256_RATE, crypto_hash_sha3256_BYTES);
}

int
crypto_hash_sha3256_update(crypto_hash_sha3256_state *state,
                           const unsigned char *in, unsigned long long inlen)
{
    return sha3_update((sha3_state_internal *) (void *) state, in, (size_t) inlen);
}

int
crypto_hash_sha3256_final(crypto_hash_sha3256_state *state, unsigned char *out)
{
    return sha3_final((sha3_state_internal *) (void *) state, out);
}

int
crypto_hash_sha3256(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    crypto_hash_sha3256_state state;

    crypto_hash_sha3256_init(&state);
    crypto_hash_sha3256_update(&state, in, inlen);
    crypto_hash_sha3256_final(&state, out);

    return 0;
}

size_t
crypto_hash_sha3512_bytes(void)
{
    return crypto_hash_sha3512_BYTES;
}

size_t
crypto_hash_sha3512_statebytes(void)
{
    return sizeof(crypto_hash_sha3512_state);
}

int
crypto_hash_sha3512_init(crypto_hash_sha3512_state *state)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3512_state) >= sizeof(sha3_state_internal));
    return sha3_init((sha3_state_internal *) (void *) state,
                     SHA3_512_RATE, crypto_hash_sha3512_BYTES);
}

int
crypto_hash_sha3512_update(crypto_hash_sha3512_state *state,
                           const unsigned char *in, unsigned long long inlen)
{
    return sha3_update((sha3_state_internal *) (void *) state, in, (size_t) inlen);
}

int
crypto_hash_sha3512_final(crypto_hash_sha3512_state *state, unsigned char *out)
{
    return sha3_final((sha3_state_internal *) (void *) state, out);
}

int
crypto_hash_sha3512(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    crypto_hash_sha3512_state state;

    crypto_hash_sha3512_init(&state);
    crypto_hash_sha3512_update(&state, in, inlen);
    crypto_hash_sha3512_final(&state, out);

    return 0;
}
