
#include "crypto_sign.h"

int
crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                         const unsigned char *seed)
{
    return crypto_sign_ed25519_seed_keypair(pk, sk, seed);
}

int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    return crypto_sign_ed25519_keypair(pk, sk);
}

int
crypto_sign(unsigned char *sm, unsigned long long *smlen,
            const unsigned char *m, unsigned long long mlen,
            const unsigned char *sk)
{
    return crypto_sign_ed25519(sm, smlen, m, mlen, sk);
}

int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk)
{
    return crypto_sign_ed25519_open(m, mlen, sm, smlen, pk);
}
