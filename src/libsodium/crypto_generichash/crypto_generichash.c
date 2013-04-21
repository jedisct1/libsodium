
#include "crypto_generichash.h"

int
crypto_generichash(unsigned char *out, const unsigned char *in,
                   const unsigned char *key,
                   size_t outlen, unsigned long long inlen, size_t keylen)
{
    return crypto_generichash_blake2b(out, in, key, outlen, inlen, keylen);
}

int
crypto_generichash_init(crypto_generichash_blake2b_state *state,
                        const unsigned char *key,
                        const size_t keylen, const size_t outlen)
{
    return crypto_generichash_blake2b_init(state, key, keylen, outlen);
}
    
int
crypto_generichash_update(crypto_generichash_blake2b_state *state,
                          const unsigned char *in,
                          unsigned long long inlen)
{
    return crypto_generichash_blake2b_update(state, in, inlen);
}

int
crypto_generichash_final(crypto_generichash_blake2b_state *state,
                         unsigned char *out, const size_t outlen)
{
    return crypto_generichash_blake2b_final(state, out, outlen);
}
