
#define TEST_NAME "shorthash"
#include "cmptest.h"

int main(void)
{
#define MAXLEN 64
    unsigned char in[MAXLEN], out[crypto_shorthash_BYTES],
        k[crypto_shorthash_KEYBYTES];
    size_t i, j;

    for (i = 0; i < crypto_shorthash_KEYBYTES; ++i)
        k[i] = i;

    for (i = 0; i < MAXLEN; ++i) {
        in[i] = i;
        crypto_shorthash(out, in, i, k);
        for (j = 0; j < crypto_shorthash_BYTES; ++j)
            printf("%02x", (unsigned int)out[j]);
        printf("\n");
    }
    assert(crypto_shorthash_bytes() > 0);
    assert(crypto_shorthash_keybytes() > 0);
    assert(strcmp(crypto_shorthash_primitive(), "siphash24") == 0);
    assert(crypto_shorthash_bytes() == crypto_shorthash_siphash24_bytes());
    assert(crypto_shorthash_keybytes()
           == crypto_shorthash_siphash24_keybytes());

    return 0;
}
