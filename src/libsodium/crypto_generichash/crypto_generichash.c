
#include "crypto_generichash.h"

size_t
crypto_generichash_bytes(void)
{
    return crypto_generichash_BYTES;
}

size_t
crypto_generichash_bytes_min(void)
{
    return crypto_generichash_BYTES_MIN;
}

size_t
crypto_generichash_bytes_max(void)
{
    return crypto_generichash_BYTES_MAX;
}

size_t
crypto_generichash_keybytes(void)
{
    return crypto_generichash_KEYBYTES;
}

size_t
crypto_generichash_keybytes_min(void)
{
    return crypto_generichash_KEYBYTES_MIN;
}

size_t
crypto_generichash_keybytes_max(void)
{
    return crypto_generichash_KEYBYTES_MAX;
}

size_t
crypto_generichash_blockbytes(void)
{
    return crypto_generichash_BLOCKBYTES;
}

const char *crypto_generichash_primitive(void)
{
    return crypto_generichash_PRIMITIVE;
}

int
crypto_generichash(unsigned char *out, size_t outlen, const unsigned char *in,
                   uint64_t inlen, const unsigned char *key,
                   size_t keylen)
{
    return crypto_generichash_blake2b(out, outlen, in, inlen, key, keylen);
}

int
crypto_generichash_init(crypto_generichash_state *state,
                        const unsigned char *key,
                        const size_t keylen, const size_t outlen)
{
    return crypto_generichash_blake2b_init
        ((crypto_generichash_blake2b_state *) state,
            key, keylen, outlen);
}

int
crypto_generichash_update(crypto_generichash_state *state,
                          const unsigned char *in,
                          uint64_t inlen)
{
    return crypto_generichash_blake2b_update
        ((crypto_generichash_blake2b_state *) state, in, inlen);
}

int
crypto_generichash_final(crypto_generichash_state *state,
                         unsigned char *out, const size_t outlen)
{
    return crypto_generichash_blake2b_final
        ((crypto_generichash_blake2b_state *) state, out, outlen);
}
