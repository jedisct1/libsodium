
#define TEST_NAME "otp"
#include "cmptest.h"

#include <stdio.h>

// Test vectors taken from RFC 6238, Appendix B.

int main(void)
{
    const char key256[32] = "12345678901234567890123456789012";
    const char key512[64] = "1234567890123456789012345678901234567890123456789012345678901234";
    const uint64_t initialTime = 0;
    const uint64_t step = 30;
    const int digits = 8;
    const uint64_t times[] = { 59ul, 1111111109ul, 1111111111ul, 1234567890ul, 2000000000ul, 20000000000ul };
    const size_t times_size = 6;

    int i;
    char* buf = (char*)malloc(digits + 1);

    // Sha256 tests
    for (i = 0; i < times_size; i++) {
        crypto_totp_sha256(key256, 32, times[i], initialTime, step, digits, buf);
        printf("%s\n", buf);
    }

    // Sha512 tests
    for (i = 0; i < times_size; i++) {
        crypto_totp_sha512(key512, 64, times[i], initialTime, step, digits, buf);
        printf("%s\n", buf);
    }

    return 0;
}
