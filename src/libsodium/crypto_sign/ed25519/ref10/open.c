
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "api.h"
#include "crypto_hash_sha512.h"
#include "crypto_verify_32.h"
#include "ge.h"
#include "sc.h"
#include "utils.h"

int
crypto_sign_verify_beforenm(unsigned char *A, const unsigned char *pk)
{
    unsigned int i;
    unsigned char d = 0;

    for (i = 0; i < 32; ++i) {
        d |= pk[i];
    }
    if (d == 0) {
        return -1;
    }
    if (ge_frombytes_negate_vartime((ge_p3 *)A, pk) != 0) {
        return -1;
    }

    return 0;
}

int
crypto_sign_verify_detached_afternm(const unsigned char *sig, const unsigned char *m,
                                    unsigned long long mlen, const unsigned char *A,
                                    const unsigned char *pk)
{
    crypto_hash_sha512_state hs;
    unsigned char h[64];
    unsigned char rcheck[32];
    unsigned int  i;
    ge_p2 R;

    if (sig[63] & 224) {
        return -1;
    }

    crypto_hash_sha512_init(&hs);
    crypto_hash_sha512_update(&hs, sig, 32);
    crypto_hash_sha512_update(&hs, pk, 32);
    crypto_hash_sha512_update(&hs, m, mlen);
    crypto_hash_sha512_final(&hs, h);
    sc_reduce(h);

    ge_double_scalarmult_vartime(&R, h, (ge_p3 *)A, sig + 32);
    ge_tobytes(rcheck, &R);

    return crypto_verify_32(rcheck, sig) | (-(rcheck == sig)) |
           sodium_memcmp(sig, rcheck, 32);
}

int
crypto_sign_verify_detached(const unsigned char *sig, const unsigned char *m,
                            unsigned long long mlen, const unsigned char *pk)
{
    ge_p3 A;

    if (crypto_sign_verify_beforenm((unsigned char *)&A, pk)) {
        return -1;
    }

    return crypto_sign_verify_detached_afternm(sig, m, mlen, (unsigned char *)&A, pk);
}

int
crypto_sign_open(unsigned char *m, unsigned long long *mlen_p,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk)
{
    unsigned long long mlen;

    if (smlen < 64 || smlen > SIZE_MAX) {
        goto badsig;
    }
    mlen = smlen - 64;
    if (crypto_sign_verify_detached(sm, sm + 64, mlen, pk) != 0) {
        memset(m, 0, mlen);
        goto badsig;
    }
    if (mlen_p != NULL) {
        *mlen_p = mlen;
    }
    memmove(m, sm + 64, mlen);

    return 0;

badsig:
    if (mlen_p != NULL) {
        *mlen_p = 0;
    }
    return -1;
}
