
#define TEST_NAME "pwbox"
#include "cmptest.h"

#define OPSLIMIT 2
#define MEMLIMIT 500000


int
main(void)
{
    unsigned char *m;
    unsigned char *m2;
    unsigned char *c;
    unsigned char *mac;
    size_t         mlen;
    size_t         i;
    const char    *pw;
    int            pwlen;

    mlen  = (size_t) randombytes_uniform((uint32_t) 10000) + 1U;
    m     = (unsigned char *) sodium_malloc(mlen);
    m2    = (unsigned char *) sodium_malloc(mlen);
    c     = (unsigned char *) sodium_malloc(crypto_pwbox_MACBYTES + mlen);
    mac   = (unsigned char *) sodium_malloc(crypto_pwbox_MACBYTES);
    pw    = "password";
    pwlen = strlen(pw);

    randombytes_buf(m, mlen);
    crypto_pwbox(c, m, (unsigned long long) mlen, pw, pwlen,
                 OPSLIMIT, MEMLIMIT);
    if (crypto_pwbox_open(m2, c,
                          (unsigned long long) mlen + crypto_pwbox_MACBYTES,
                          pw, pwlen) != 0) {
        printf("crypto_pwbox_open() failed\n");
        return 1;
    }
    printf("%d\n", memcmp(m, m2, mlen));

    if (crypto_pwbox_open(m2, c, 0, pw, pwlen) == 0) {
        printf("short open() should have failed\n");
        return 1;
    }
    if (crypto_pwbox_open(m2, c, mlen + crypto_pwbox_MACBYTES - 1,
                          pw, pwlen) == 0) {
        printf("short open() should have failed\n");
        return 1;
    }
    crypto_pwbox_detached_alg(c, mac, m, (unsigned long long) mlen, pw, pwlen,
                              OPSLIMIT, MEMLIMIT,
                              crypto_pwbox_alg_argon2id13());

    if (crypto_pwbox_open_detached(NULL, c, mac, (unsigned long long) mlen,
                                   pw, pwlen) != 0) {
        printf("crypto_pwbox_open_detached() with a NULL message pointer failed\n");
    }
    if (crypto_pwbox_open_detached(m2, c, mac, (unsigned long long) mlen,
                                   pw, pwlen) != 0) {
        printf("crypto_pwbox_open_detached() failed\n");
    }
    printf("%d\n", memcmp(m, m2, mlen));

    memcpy(c, m, mlen);
    crypto_pwbox(c, c, (unsigned long long) mlen, pw, pwlen, OPSLIMIT,
                 MEMLIMIT);
    printf("%d\n", memcmp(m, c, mlen) == 0);
    printf("%d\n", memcmp(m, c + crypto_pwbox_MACBYTES, mlen) == 0);
    if (crypto_pwbox_open(c, c,
                          (unsigned long long) mlen + crypto_pwbox_MACBYTES,
                          pw, pwlen) != 0) {
        printf("crypto_pwbox_open() failed\n");
    }
    printf("%d\n", memcmp(m, c, mlen));

    sodium_free(m);
    sodium_free(m2);
    sodium_free(c);
    sodium_free(mac);

    return 0;
}
