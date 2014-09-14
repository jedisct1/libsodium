#include "randombytes.h"

unsigned char x[65536];
unsigned long long freq[256];

static int nacl_tests(void)
{
    unsigned long long i;

    randombytes(x, sizeof x);
    for (i = 0; i < 256; ++i) {
        freq[i] = 0;
    }
    for (i = 0; i < sizeof x; ++i) {
        ++freq[255 & (int)x[i]];
    }
    for (i = 0; i < 256; ++i) {
        if (!freq[i]) {
            return 111;
        }
    }
}

int main(void)
{
    nacl_tests();

    return 0;
}
