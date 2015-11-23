
#define TEST_NAME "box8"
#include "cmptest.h"

static unsigned char alicesk[crypto_box_SECRETKEYBYTES];
static unsigned char alicepk[crypto_box_PUBLICKEYBYTES];
static unsigned char bobsk[crypto_box_SECRETKEYBYTES];
static unsigned char bobpk[crypto_box_PUBLICKEYBYTES];
static unsigned char n[crypto_box_NONCEBYTES];
static unsigned char m[10000];
static unsigned char c[10000];
static unsigned char m2[10000];

int main(void)
{
    size_t mlen;
    size_t i;
    int    caught;
    int    ret;

    for (mlen = 0; mlen < 1000 && mlen + crypto_box_ZEROBYTES < sizeof m;
         ++mlen) {
        crypto_box_keypair(alicepk, alicesk);
        crypto_box_keypair(bobpk, bobsk);
        randombytes_buf(n, crypto_box_NONCEBYTES);
        randombytes_buf(m + crypto_box_ZEROBYTES, mlen);
        ret = crypto_box(c, m, mlen + crypto_box_ZEROBYTES, n, bobpk, alicesk);
        assert(ret == 0);
        caught = 0;
        while (caught < 10) {
            c[rand() % (mlen + crypto_box_ZEROBYTES)] = rand();
            if (crypto_box_open(m2, c, mlen + crypto_box_ZEROBYTES, n, alicepk,
                                bobsk) == 0) {
                for (i = 0; i < mlen + crypto_box_ZEROBYTES; ++i) {
                    if (m2[i] != m[i]) {
                        printf("forgery\n");
                        return 100;
                    }
                }
            } else {
                ++caught;
            }
        }
    }
    return 0;
}
