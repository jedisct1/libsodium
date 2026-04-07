#define TEST_NAME "kem_xwing"
#include "cmptest.h"

static const char tv0_seed[] =
    "0000000000000000000000000000000000000000000000000000000000000000";
static const char tv0_randomness[] =
    "6464646464646464646464646464646464646464646464646464646464646464"
    "6464646464646464646464646464646464646464646464646464646464646464";
static const char tv0_ek_prefix[] =
    "3d209f716752f6408e7f89bceef97ac388530045377927644ef046c0a7cae978";
static const char tv0_ct_prefix[] =
    "d81018a94f8078e02105beaa814e003390befa4589bb614f7739";
static const char tv0_ss[] =
    "e5ba94031ea6efd69c09c254f6d9783136ba6037e2d4c43bcccf19d6f3f4343a";

static const char tv1_seed[] =
    "0101010101010101010101010101010101010101010101010101010101010101";
static const char tv1_randomness[] =
    "6565656565656565656565656565656565656565656565656565656565656565"
    "6565656565656565656565656565656565656565656565656565656565656565";
static const char tv1_ek_prefix[] =
    "ec7b50cddc8360f98b189bac73d395ef947b37d8453886a253269f7b18b9eb78";
static const char tv1_ct_prefix[] =
    "600ecf4026683898d0e339eeea9ebd437a4a802952bf32bfa326";
static const char tv1_ss[] =
    "750300db25bff9620e893c2c6fcab9bf04d7f2e543b5b39420485626fa274908";

static const char tv2_seed[] =
    "0202020202020202020202020202020202020202020202020202020202020202";
static const char tv2_randomness[] =
    "6666666666666666666666666666666666666666666666666666666666666666"
    "6666666666666666666666666666666666666666666666666666666666666666";
static const char tv2_ek_prefix[] =
    "08118d8819772292c976ec971ee3039195800c823544484595cc63450b9db941";
static const char tv2_ct_prefix[] =
    "413c55d5710bae6376761dada807daffd4dc45f9f70d825e0d46";
static const char tv2_ss[] =
    "87292f18b2e7af74bb8839ddee15e832d2f4bfac14dc84f824906d951436aafa";

static int
test_kat_vector(const char *seed_hex, const char *randomness_hex,
                const char *ek_prefix_hex, const char *ct_prefix_hex,
                const char *ss_hex)
{
    unsigned char seed[crypto_kem_xwing_SEEDBYTES];
    unsigned char pk[crypto_kem_xwing_PUBLICKEYBYTES];
    unsigned char sk[crypto_kem_xwing_SECRETKEYBYTES];
    unsigned char randomness[64];
    unsigned char ct[crypto_kem_xwing_CIPHERTEXTBYTES];
    unsigned char ss_enc[crypto_kem_xwing_SHAREDSECRETBYTES];
    unsigned char ss_dec[crypto_kem_xwing_SHAREDSECRETBYTES];
    unsigned char expected_ek_prefix[32];
    unsigned char expected_ct_prefix[26];
    unsigned char expected_ss[32];
    char          hex[65];

    sodium_hex2bin(seed, sizeof seed, seed_hex, strlen(seed_hex), NULL, NULL, NULL);
    sodium_hex2bin(randomness, sizeof randomness, randomness_hex, strlen(randomness_hex),
                   NULL, NULL, NULL);
    sodium_hex2bin(expected_ek_prefix, sizeof expected_ek_prefix, ek_prefix_hex,
                   strlen(ek_prefix_hex), NULL, NULL, NULL);
    sodium_hex2bin(expected_ct_prefix, sizeof expected_ct_prefix, ct_prefix_hex,
                   strlen(ct_prefix_hex), NULL, NULL, NULL);
    sodium_hex2bin(expected_ss, sizeof expected_ss, ss_hex, strlen(ss_hex), NULL, NULL, NULL);

    crypto_kem_xwing_seed_keypair(pk, sk, seed);

    if (memcmp(pk, expected_ek_prefix, 32) != 0) {
        sodium_bin2hex(hex, sizeof hex, pk, 32);
        printf("pk mismatch: got %s\n", hex);
        return -1;
    }

    assert(crypto_kem_xwing_enc_deterministic(ct, ss_enc, pk, randomness) == 0);

    if (memcmp(ct, expected_ct_prefix, 26) != 0) {
        sodium_bin2hex(hex, sizeof hex, ct, 26);
        printf("ct mismatch: got %s\n", hex);
        return -1;
    }

    if (memcmp(ss_enc, expected_ss, 32) != 0) {
        sodium_bin2hex(hex, sizeof hex, ss_enc, 32);
        printf("ss_enc mismatch: got %s\n", hex);
        return -1;
    }

    assert(crypto_kem_xwing_dec(ss_dec, ct, sk) == 0);

    if (memcmp(ss_dec, expected_ss, 32) != 0) {
        sodium_bin2hex(hex, sizeof hex, ss_dec, 32);
        printf("ss_dec mismatch: got %s\n", hex);
        return -1;
    }

    return 0;
}

static void
tv_kem_xwing(void)
{
    unsigned char *pk;
    unsigned char *sk;
    unsigned char *seed;
    unsigned char *ct;
    unsigned char *ss_enc;
    unsigned char *ss_dec;
    unsigned char *randomness;
    char           hex[65];
    int            i;

    pk = (unsigned char *) sodium_malloc(crypto_kem_xwing_PUBLICKEYBYTES);
    sk = (unsigned char *) sodium_malloc(crypto_kem_xwing_SECRETKEYBYTES);
    seed = (unsigned char *) sodium_malloc(crypto_kem_xwing_SEEDBYTES);
    ct = (unsigned char *) sodium_malloc(crypto_kem_xwing_CIPHERTEXTBYTES);
    ss_enc = (unsigned char *) sodium_malloc(crypto_kem_xwing_SHAREDSECRETBYTES);
    ss_dec = (unsigned char *) sodium_malloc(crypto_kem_xwing_SHAREDSECRETBYTES);
    randomness = (unsigned char *) sodium_malloc(64);

    for (i = 0; i < (int) crypto_kem_xwing_SEEDBYTES; i++) {
        seed[i] = (unsigned char) i;
    }
    crypto_kem_xwing_seed_keypair(pk, sk, seed);

    sodium_bin2hex(hex, sizeof hex, pk, 32);
    printf("pk (first 32 bytes): [%s]\n", hex);
    sodium_bin2hex(hex, sizeof hex, sk, 32);
    printf("sk: [%s]\n", hex);

    for (i = 0; i < 64; i++) {
        randomness[i] = (unsigned char)(i + 64);
    }
    assert(crypto_kem_xwing_enc_deterministic(ct, ss_enc, pk, randomness) == 0);

    sodium_bin2hex(hex, sizeof hex, ct, 32);
    printf("ct (first 32 bytes): [%s]\n", hex);
    sodium_bin2hex(hex, sizeof hex, ss_enc, crypto_kem_xwing_SHAREDSECRETBYTES);
    printf("ss_enc: [%s]\n", hex);

    assert(crypto_kem_xwing_dec(ss_dec, ct, sk) == 0);

    sodium_bin2hex(hex, sizeof hex, ss_dec, crypto_kem_xwing_SHAREDSECRETBYTES);
    printf("ss_dec: [%s]\n", hex);

    if (memcmp(ss_enc, ss_dec, crypto_kem_xwing_SHAREDSECRETBYTES) != 0) {
        printf("ERROR: shared secrets don't match!\n");
    } else {
        printf("shared secrets match: ok\n");
    }

    crypto_kem_xwing_keypair(pk, sk);
    assert(crypto_kem_xwing_enc(ct, ss_enc, pk) == 0);
    assert(crypto_kem_xwing_dec(ss_dec, ct, sk) == 0);
    if (memcmp(ss_enc, ss_dec, crypto_kem_xwing_SHAREDSECRETBYTES) != 0) {
        printf("ERROR: shared secrets don't match (random keys)!\n");
    } else {
        printf("random keypair test: ok\n");
    }

    assert(crypto_kem_xwing_publickeybytes() == crypto_kem_xwing_PUBLICKEYBYTES);
    assert(crypto_kem_xwing_secretkeybytes() == crypto_kem_xwing_SECRETKEYBYTES);
    assert(crypto_kem_xwing_ciphertextbytes() == crypto_kem_xwing_CIPHERTEXTBYTES);
    assert(crypto_kem_xwing_sharedsecretbytes() == crypto_kem_xwing_SHAREDSECRETBYTES);
    assert(crypto_kem_xwing_seedbytes() == crypto_kem_xwing_SEEDBYTES);

    assert(crypto_kem_xwing_PUBLICKEYBYTES == 1216U);
    assert(crypto_kem_xwing_SECRETKEYBYTES == 32U);
    assert(crypto_kem_xwing_CIPHERTEXTBYTES == 1120U);
    assert(crypto_kem_xwing_SHAREDSECRETBYTES == 32U);
    assert(crypto_kem_xwing_SEEDBYTES == 32U);

    printf("constants: ok\n");

    sodium_free(randomness);
    sodium_free(ss_dec);
    sodium_free(ss_enc);
    sodium_free(ct);
    sodium_free(seed);
    sodium_free(sk);
    sodium_free(pk);

    printf("tv_kem_xwing: ok\n");
}

static void
tv_kem_xwing_kat(void)
{
    printf("IETF KAT vector 0: ");
    if (test_kat_vector(tv0_seed, tv0_randomness, tv0_ek_prefix, tv0_ct_prefix, tv0_ss) == 0) {
        printf("ok\n");
    } else {
        printf("FAILED\n");
    }

    printf("IETF KAT vector 1: ");
    if (test_kat_vector(tv1_seed, tv1_randomness, tv1_ek_prefix, tv1_ct_prefix, tv1_ss) == 0) {
        printf("ok\n");
    } else {
        printf("FAILED\n");
    }

    printf("IETF KAT vector 2: ");
    if (test_kat_vector(tv2_seed, tv2_randomness, tv2_ek_prefix, tv2_ct_prefix, tv2_ss) == 0) {
        printf("ok\n");
    } else {
        printf("FAILED\n");
    }
}

int
main(void)
{
    tv_kem_xwing();
    tv_kem_xwing_kat();

    return 0;
}
