#define TEST_NAME "kem"
#include "cmptest.h"

static void
tv_kem(void)
{
    unsigned char *pk;
    unsigned char *sk;
    unsigned char *seed;
    unsigned char *ct;
    unsigned char *ss_enc;
    unsigned char *ss_dec;
    int            i;

    pk = (unsigned char *) sodium_malloc(crypto_kem_PUBLICKEYBYTES);
    sk = (unsigned char *) sodium_malloc(crypto_kem_SECRETKEYBYTES);
    seed = (unsigned char *) sodium_malloc(crypto_kem_SEEDBYTES);
    ct = (unsigned char *) sodium_malloc(crypto_kem_CIPHERTEXTBYTES);
    ss_enc = (unsigned char *) sodium_malloc(crypto_kem_SHAREDSECRETBYTES);
    ss_dec = (unsigned char *) sodium_malloc(crypto_kem_SHAREDSECRETBYTES);

    printf("primitive: %s\n", crypto_kem_primitive());

    assert(crypto_kem_publickeybytes() == crypto_kem_PUBLICKEYBYTES);
    assert(crypto_kem_secretkeybytes() == crypto_kem_SECRETKEYBYTES);
    assert(crypto_kem_ciphertextbytes() == crypto_kem_CIPHERTEXTBYTES);
    assert(crypto_kem_sharedsecretbytes() == crypto_kem_SHAREDSECRETBYTES);
    assert(crypto_kem_seedbytes() == crypto_kem_SEEDBYTES);

    printf("constants: ok\n");

    for (i = 0; i < (int) crypto_kem_SEEDBYTES; i++) {
        seed[i] = (unsigned char) i;
    }
    crypto_kem_seed_keypair(pk, sk, seed);

    assert(crypto_kem_enc(ct, ss_enc, pk) == 0);
    assert(crypto_kem_dec(ss_dec, ct, sk) == 0);

    if (memcmp(ss_enc, ss_dec, crypto_kem_SHAREDSECRETBYTES) != 0) {
        printf("ERROR: shared secrets don't match!\n");
    } else {
        printf("shared secrets match: ok\n");
    }

    crypto_kem_keypair(pk, sk);
    assert(crypto_kem_enc(ct, ss_enc, pk) == 0);
    assert(crypto_kem_dec(ss_dec, ct, sk) == 0);

    if (memcmp(ss_enc, ss_dec, crypto_kem_SHAREDSECRETBYTES) != 0) {
        printf("ERROR: shared secrets don't match (random keys)!\n");
    } else {
        printf("random keypair test: ok\n");
    }

    sodium_free(ss_dec);
    sodium_free(ss_enc);
    sodium_free(ct);
    sodium_free(seed);
    sodium_free(sk);
    sodium_free(pk);
}

int
main(void)
{
    tv_kem();

    return 0;
}
