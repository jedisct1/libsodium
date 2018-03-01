/* "Test Case AUTH256-4" from RFC 4868 */

#define TEST_NAME "auth3"
#ifndef SGX
#include "cmptest.h"
#else
#include "test_enclave.h"
#endif

static unsigned char key[32] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
};

static unsigned char c[50] = { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
                               0xcd, 0xcd };

static unsigned char a[32] = { 0x37, 0x2e, 0xfc, 0xf9, 0xb4, 0x0b, 0x35, 0xc2,
                               0x11, 0x5b, 0x13, 0x46, 0x90, 0x3d, 0x2e, 0xf4,
                               0x2f, 0xce, 0xd4, 0x6f, 0x08, 0x46, 0xe7, 0x25,
                               0x7b, 0xb1, 0x56, 0xd3, 0xd7, 0xb3, 0x0d, 0x3f };

int
main(void)
{
    printf("%d\n", crypto_auth_hmacsha256_verify(a, c, sizeof c, key));

    return 0;
}
