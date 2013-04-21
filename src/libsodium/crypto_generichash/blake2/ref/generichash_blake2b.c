
#include <assert.h>
#include <limits.h>
#include <stdint.h>

#include "api.h"
#include "blake2.h"

int
crypto_generichash_blake2b(unsigned char *out, size_t outlen,
                           const unsigned char *in, unsigned long long inlen,
                           const unsigned char *key, size_t keylen)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES || inlen > UINT64_MAX) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);

    return blake2b((uint8_t *) out, in, key,
                   (uint8_t) outlen, (uint64_t) inlen, (uint8_t) keylen);
}

int
crypto_generichash_blake2b_init(crypto_generichash_blake2b_state *state,
                                const unsigned char *key,
                                const size_t keylen, const size_t outlen)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);
    if (blake2b_init(state, (uint8_t) outlen) != 0) {
        return -1;
    }
    if (key != NULL && keylen > 0U &&
        blake2b_init_key(state, (uint8_t) outlen, key, keylen) != 0) {
        return -1;
    }
    return 0;
}

int
crypto_generichash_blake2b_update(crypto_generichash_blake2b_state *state,
                                  const unsigned char *in,
                                  unsigned long long inlen)
{
    return blake2b_update(state, (const uint8_t *) in, (uint64_t) inlen);
}

int
crypto_generichash_blake2b_final(crypto_generichash_blake2b_state *state,
                                 unsigned char *out,
                                 const size_t outlen)
{
    assert(outlen <= UINT8_MAX);
    return blake2b_final(state, (uint8_t *) out, (uint8_t) outlen);
}
