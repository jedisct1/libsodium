
#define TEST_NAME "secretbox_easy2"
#include "cmptest.h"

unsigned char m[10000];
unsigned char m2[10000];
unsigned char c[crypto_secretbox_MACBYTES + 10000];
unsigned char nonce[crypto_secretbox_NONCEBYTES];
unsigned char k[crypto_secretbox_KEYBYTES];
unsigned char mac[crypto_secretbox_MACBYTES];

int main(void)
{
    unsigned long long mlen;
    unsigned long long i;

    randombytes_buf(k, sizeof k);
    mlen = (unsigned long long)randombytes_uniform((uint32_t)sizeof m);
    randombytes_buf(m, mlen);
    randombytes_buf(nonce, sizeof nonce);
    crypto_secretbox_easy(c, m, mlen, nonce, k);
    crypto_secretbox_open_easy(m2, c, mlen + crypto_secretbox_MACBYTES, nonce,
                               k);
    printf("%d\n", memcmp(m, m2, mlen));

    for (i = 0; i < mlen + crypto_secretbox_MACBYTES - 1; i++) {
        if (crypto_secretbox_open_easy(m2, c, i, nonce, k) == 0) {
            printf("short open() should have failed");
            return 1;
        }
    }
    crypto_secretbox_detached(c, mac, m, mlen, nonce, k);
    crypto_secretbox_open_detached(m2, c, mac, mlen, nonce, k);
    printf("%d\n", memcmp(m, m2, mlen));

    return 0;
}
