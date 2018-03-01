
#define TEST_NAME "hash3"
#ifndef SGX
#include "cmptest.h"
#else
#include "test_enclave.h"
#endif

static unsigned char x[] = "testing\n";
static unsigned char h[crypto_hash_BYTES];

int
main(void)
{
    size_t i;

    crypto_hash(h, x, sizeof x - 1U);
    for (i = 0; i < crypto_hash_BYTES; ++i) {
        printf("%02x", (unsigned int) h[i]);
    }
    printf("\n");

    return 0;
}
