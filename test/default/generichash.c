#include <stdio.h>

#include "crypto_uint8.h"

#define TEST_NAME "generichash"
#include "cmptest.h"

int main(void)
{
#define MAXLEN 64
    crypto_uint8 in[MAXLEN], out[crypto_generichash_BYTES_MAX], k[crypto_generichash_KEYBYTES_MAX];
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
    return 0;
}
