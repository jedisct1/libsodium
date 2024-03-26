#include <string.h>
#include <stdlib.h>

#include "crypto_hash_sha512.h"
#include "crypto_vrf_rfc9381.h"
#include "private/ed25519_ref10.h"
#include "utils.h"
#include "vrf_rfc9381.h"


int
crypto_vrf_rfc9381_prove(unsigned char *proof,
                             const unsigned char *m, unsigned long long mlen,
                             const unsigned char *sk)
{

    crypto_hash_sha512_state hs;
    unsigned char az[64];
    unsigned char H_string[32];
    unsigned char kB_string[32], kH_string[32];
    unsigned char string_to_hash[32 + mlen];
    unsigned char challenge[64], nonce[64];
    ge25519_p3    H, Gamma, kB, kH;

    crypto_hash_sha512(az, sk, 32);
    az[0] &= 248;
    az[31] &= 127;
    az[31] |= 64;

    memmove(string_to_hash, sk + 32, 32);
    memmove(string_to_hash + 32, m, mlen);
    ge25519_from_string(H_string, "ECVRF_edwards25519_XMD:SHA-512_ELL2_NU_\4", string_to_hash, 32 + mlen, 2); /* elligator2 */

    ge25519_frombytes(&H, H_string);
    ge25519_scalarmult(&Gamma, az, &H);

    crypto_hash_sha512_init(&hs);
    crypto_hash_sha512_update(&hs, az + 32, 32);
    crypto_hash_sha512_update(&hs, H_string, 32);
    crypto_hash_sha512_final(&hs, nonce);

    sc25519_reduce(nonce);
    ge25519_scalarmult_base(&kB, nonce);
    ge25519_scalarmult(&kH, nonce, &H);

    ge25519_p3_tobytes(proof, &Gamma);
    ge25519_p3_tobytes(kB_string, &kB);
    ge25519_p3_tobytes(kH_string, &kH);

    crypto_hash_sha512_init(&hs);
    crypto_hash_sha512_update(&hs, &SUITE, 1);
    crypto_hash_sha512_update(&hs, &TWO, 1);
    crypto_hash_sha512_update(&hs, sk + 32, 32);
    crypto_hash_sha512_update(&hs, H_string, 32);
    crypto_hash_sha512_update(&hs, proof, 32);
    crypto_hash_sha512_update(&hs, kB_string, 32);
    crypto_hash_sha512_update(&hs, kH_string, 32);
    crypto_hash_sha512_update(&hs, &ZERO, 1);
    crypto_hash_sha512_final(&hs, challenge);

    memmove(proof + 32, challenge, 16);
    memset(challenge + 16, 0, 48); /* we zero out the last 48 bytes of the challenge */
    sc25519_muladd(proof + 48, challenge, az, nonce);

    sodium_memzero(az, sizeof az);
    sodium_memzero(nonce, sizeof nonce);

    return 0;
}
