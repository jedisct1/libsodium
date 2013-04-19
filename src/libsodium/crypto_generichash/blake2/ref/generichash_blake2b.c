
#include <assert.h>
#include <limits.h>
#include <stdint.h>

#include "api.h"
#include "blake2.h"

int
crypto_generichash_blake2b(unsigned char *out, const unsigned char *in,
                           const unsigned char *key,
                           size_t outlen, unsigned long long inlen,
                           size_t keylen)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES || inlen > UINT64_MAX) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);

    return blake2b((uint8_t *) out, in, key,
                   (uint8_t) outlen, inlen, (uint8_t) keylen);
}
