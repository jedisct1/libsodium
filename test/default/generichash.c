
#define TEST_NAME "generichash"
#include "cmptest.h"

int main(void)
{
#define MAXLEN 64
    unsigned char in[MAXLEN], out[crypto_generichash_BYTES_MAX], k[crypto_generichash_KEYBYTES_MAX];
    size_t h,i,j;

    for(h = 0; h < crypto_generichash_KEYBYTES_MAX; ++h) k[h] = h;

    for(i = 0; i < MAXLEN; ++i) {
        in[i]=i;
        crypto_generichash(out, 1 + i % crypto_generichash_BYTES_MAX,
                           in, i,
                           k, 1 + i % crypto_generichash_KEYBYTES_MAX);
        for (j = 0;j < 1 + i % crypto_generichash_BYTES_MAX;++j) {
            printf("%02x",(unsigned int) out[j]);
        }
        printf("\n");
    }

    assert(crypto_generichash_bytes_min() > 0U);
    assert(crypto_generichash_bytes_max() > 0U);
    assert(crypto_generichash_bytes() > 0U);
    assert(crypto_generichash_bytes() >= crypto_generichash_bytes_min());
    assert(crypto_generichash_bytes() <= crypto_generichash_bytes_max());
    assert(crypto_generichash_keybytes_min() >= 0U);
    assert(crypto_generichash_keybytes_max() > 0U);
    assert(crypto_generichash_keybytes() > 0U);
    assert(crypto_generichash_keybytes() >= crypto_generichash_keybytes_min());
    assert(crypto_generichash_keybytes() <= crypto_generichash_keybytes_max());
    assert(strcmp(crypto_generichash_primitive(), "blake2b") == 0);

    return 0;
}
