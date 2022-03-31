#define TEST_NAME "shifumi128"
#include "cmptest.h"

int
main(void)
{
    unsigned char                    secret[crypto_zkp_shifumi128_SECRETBYTES];
    unsigned char                    proof[crypto_zkp_shifumi128_BYTES];
    crypto_zkp_shifumi128_commitment move;

    crypto_zkp_shufumi128_pick(&move);
    crypto_zkp_shifumi128_commit(secret, proof, move);

    assert(crypto_zkp_shifumi128_verify(secret, proof, move) == 0);

    crypto_zkp_shifumi128_commit(secret, proof, crypto_zkp_shifumi128_ROCK);

    assert(crypto_zkp_shifumi128_verify(secret, proof,
                                        crypto_zkp_shifumi128_PAPER) == -1);
}
