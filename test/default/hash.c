
#define TEST_NAME "hash"
#include "cmptest.h"

unsigned char x[] = "testing\n";
unsigned char h[crypto_hash_BYTES];

int main(void)
{
    size_t i;

    crypto_hash(h, x, sizeof x - 1U);
    for (i = 0; i < crypto_hash_BYTES; ++i) {
        printf("%02x", (unsigned int)h[i]);
    }
    printf("\n");

    assert(crypto_hash_bytes() > 0U);
    assert(strcmp(crypto_hash_primitive(), "sha512") == 0);
    assert(crypto_hash_sha256_bytes() > 0U);
    assert(crypto_hash_sha512_bytes() == crypto_hash_bytes());

    return 0;
}
