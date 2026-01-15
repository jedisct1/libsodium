#include "crypto_hash_sha3.h"
#include "private/common.h"
#include "ref/sha3_ref.h"

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
crypto_hash_sha3256(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3256_state) >= sizeof(sha3_state_internal));
    return sha3256_ref(out, in, (size_t) inlen);
}

int
crypto_hash_sha3256_init(crypto_hash_sha3256_state *state)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3256_state) >= sizeof(sha3_state_internal));
    return sha3256_ref_init((sha3_state_internal *) (void *) state);
}

int
crypto_hash_sha3256_update(crypto_hash_sha3256_state *state,
                           const unsigned char *in, unsigned long long inlen)
{
    return sha3256_ref_update((sha3_state_internal *) (void *) state, in, (size_t) inlen);
}

int
crypto_hash_sha3256_final(crypto_hash_sha3256_state *state, unsigned char *out)
{
    return sha3256_ref_final((sha3_state_internal *) (void *) state, out);
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
crypto_hash_sha3512(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3512_state) >= sizeof(sha3_state_internal));
    return sha3512_ref(out, in, (size_t) inlen);
}

int
crypto_hash_sha3512_init(crypto_hash_sha3512_state *state)
{
    COMPILER_ASSERT(sizeof(crypto_hash_sha3512_state) >= sizeof(sha3_state_internal));
    return sha3512_ref_init((sha3_state_internal *) (void *) state);
}

int
crypto_hash_sha3512_update(crypto_hash_sha3512_state *state,
                           const unsigned char *in, unsigned long long inlen)
{
    return sha3512_ref_update((sha3_state_internal *) (void *) state, in, (size_t) inlen);
}

int
crypto_hash_sha3512_final(crypto_hash_sha3512_state *state, unsigned char *out)
{
    return sha3512_ref_final((sha3_state_internal *) (void *) state, out);
}
