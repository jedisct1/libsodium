#include <stdio.h>
#include <string.h>

#define TEST_NAME "box_easy2"
#include "cmptest.h"

unsigned char m[10000];
unsigned char m2[10000];
unsigned char c[crypto_box_MACBYTES + 10000];
unsigned char nonce[crypto_box_NONCEBYTES];
unsigned char alicepk[crypto_box_PUBLICKEYBYTES];
unsigned char alicesk[crypto_box_SECRETKEYBYTES];
unsigned char bobpk[crypto_box_PUBLICKEYBYTES];
unsigned char bobsk[crypto_box_SECRETKEYBYTES];

int main(void)
{
    unsigned long long mlen;

    crypto_box_keypair(alicepk, alicesk);
    crypto_box_keypair(bobpk, bobsk);
    mlen = (unsigned long long) randombytes_uniform((uint32_t) sizeof m);
    randombytes_buf(m, mlen);
    randombytes_buf(nonce, sizeof nonce);
    crypto_box_easy(c, m, mlen, nonce, bobpk, alicesk);
    crypto_box_open_easy(m2, c, mlen + crypto_box_MACBYTES,
                         nonce, alicepk, bobsk);
    printf("%d\n", memcmp(m, m2, mlen));

    return 0;
}
