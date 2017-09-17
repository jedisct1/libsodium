
#define TEST_NAME "metamorphic"
#include "cmptest.h"

#define MAXLEN 512
#define MAX_ITER 1000

static void
mm_1(void)
{
    crypto_generichash_state st;
    unsigned char *h, *h2;
    unsigned char *k;
    unsigned char *m;
    size_t         hlen;
    size_t         klen;
    size_t         mlen;
    size_t         l1, l2;
    int            i;

    for (i = 0; i < MAX_ITER; i++) {
        mlen = randombytes_uniform(MAXLEN);
        m = (unsigned char *) sodium_malloc(mlen);
        klen = randombytes_uniform(crypto_generichash_KEYBYTES_MAX -
                                   crypto_generichash_KEYBYTES_MIN + 1U)
            + crypto_generichash_KEYBYTES_MIN;
        k = (unsigned char *) sodium_malloc(klen);
        hlen = randombytes_uniform(crypto_generichash_BYTES_MAX -
                                   crypto_generichash_BYTES_MIN + 1U)
            + crypto_generichash_BYTES_MIN;
        h = (unsigned char *) sodium_malloc(hlen);
        h2 = (unsigned char *) sodium_malloc(hlen);

        randombytes_buf(m, mlen);
        
        crypto_generichash_init(&st, k, klen, hlen);
        l1 = randombytes_uniform(mlen);
        l2 = randombytes_uniform(mlen - l1);
        crypto_generichash_update(&st, m, l1);
        crypto_generichash_update(&st, m + l1, l2);
        crypto_generichash_update(&st, m + l1 + l2, mlen - l1 - l2);
        crypto_generichash_final(&st, h, hlen);

        crypto_generichash(h2, hlen, m, mlen, k, klen);

        assert(memcmp(h, h2, hlen) == 0);

        sodium_free(h2);
        sodium_free(h);
        sodium_free(k);
        sodium_free(m);
    }
}

int
main(void)
{
    mm_1();

    printf("OK\n");

    return 0;
}
