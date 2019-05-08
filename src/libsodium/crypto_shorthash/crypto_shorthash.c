
#include "crypto_shorthash.h"
#include "randombytes.h"

size_t
crypto_shorthash_bytes(void)
{
    return crypto_shorthash_BYTES;
}

size_t
crypto_shorthash_keybytes(void)
{
    return crypto_shorthash_KEYBYTES;
}

const char *
crypto_shorthash_primitive(void)
{
    return crypto_shorthash_PRIMITIVE;
}

int
crypto_shorthash(unsigned char *out, const unsigned char *in,
                 unsigned long long inlen, const unsigned char *k)
{
    return crypto_shorthash_siphash24(out, in, inlen, k);
}

void
crypto_shorthash_keygen(unsigned char k[crypto_shorthash_KEYBYTES])
{
    randombytes_buf(k, crypto_shorthash_KEYBYTES);
}

int
crypto_shorthash_init(crypto_shorthash_state *state,
                      const unsigned char *k)
{
    return crypto_shorthash_siphash24_init(state, k);
}

int
crypto_shorthash_update(crypto_shorthash_state *state,
                        const unsigned char *in,
                        unsigned long long inlen)
{
    return crypto_shorthash_siphash24_update(state, in, inlen);
}

int
crypto_shorthash_final(crypto_shorthash_state *state,
                       unsigned char *out)
{
    return crypto_shorthash_siphash24_final(state, out);
}
