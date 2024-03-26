#include <string.h>

#include "crypto_hash_sha512.h"
#include "crypto_vrf_rfc9381.h"
#include "private/ed25519_ref10.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_vrf_rfc9381_seed_keypair(unsigned char *pk, unsigned char *sk,
                                    const unsigned char *seed)
{
    ge25519_p3 A;

    crypto_hash_sha512(sk, seed, 32);
    sk[0] &= 248;
    sk[31] &= 127;
    sk[31] |= 64;

    ge25519_scalarmult_base(&A, sk);
    ge25519_p3_tobytes(pk, &A);

    memmove(sk, seed, 32);
    memmove(sk + 32, pk, 32);

    return 0;
}

int
crypto_vrf_rfc9381_keypair(unsigned char *pk, unsigned char *sk)
{
    unsigned char seed[32];
    int           ret;

    randombytes_buf(seed, sizeof seed);
    ret = crypto_vrf_rfc9381_seed_keypair(pk, sk, seed);
    sodium_memzero(seed, sizeof seed);

    return ret;
}
