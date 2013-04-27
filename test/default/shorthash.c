#include <stdio.h>

#include "crypto_uint8.h"

#define TEST_NAME "shorthash"
#include "cmptest.h"

int main(void)
{
#define MAXLEN 64
    crypto_uint8 in[MAXLEN], out[crypto_shorthash_BYTES], k[crypto_shorthash_KEYBYTES];
    size_t i,j;

    for( i = 0; i < crypto_shorthash_KEYBYTES; ++i ) k[i] = i;

    for(i=0;i<MAXLEN;++i) {
        in[i]=i;
        crypto_shorthash( out, in, i, k );
        for (j = 0;j < crypto_shorthash_BYTES;++j) printf("%02x",(unsigned int) out[j]);
        printf("\n");
    }
    return 0;
}
