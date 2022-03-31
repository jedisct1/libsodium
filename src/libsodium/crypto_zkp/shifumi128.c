#include "crypto_shorthash_siphash24.h"
#include "crypto_verify_16.h"
#include "crypto_zkp_shifumi128.h"
#include "randombytes.h"

size_t
crypto_zkp_shifumi128_secretbytes(void)
{
    return crypto_zkp_shifumi128_SECRETBYTES;
}

size_t
crypto_zkp_shifumi128_bytes(void)
{
    return crypto_zkp_shifumi128_BYTES;
}

void
crypto_zkp_shufumi128_pick(crypto_zkp_shifumi128_commitment *commitment)
{
    *commitment = (int) randombytes_uniform(3);
}

int
crypto_zkp_shifumi128_commit(unsigned char secret[crypto_zkp_shifumi128_SECRETBYTES],
                             unsigned char proof[crypto_zkp_shifumi128_BYTES],
                             const crypto_zkp_shifumi128_commitment commitment)
{
    const unsigned char s[1] = { (const unsigned char) commitment };

    if (commitment < 0 || commitment > 2) {
        return -1;
    }
    randombytes_buf(secret, crypto_zkp_shifumi128_SECRETBYTES);
    crypto_shorthash_siphashx24(proof, s, sizeof s, secret);

    return 0;
}

int
crypto_zkp_shifumi128_verify(const unsigned char secret[crypto_zkp_shifumi128_SECRETBYTES],
                             const unsigned char proof[crypto_zkp_shifumi128_BYTES],
                             const crypto_zkp_shifumi128_commitment commitment)
{
    const unsigned char s[1] = { (const unsigned char) commitment };
    unsigned char       computed_proof[crypto_zkp_shifumi128_BYTES];

    crypto_shorthash_siphashx24(computed_proof, s, sizeof s, secret);

    return crypto_verify_16(computed_proof, proof);
}
