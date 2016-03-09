
#define TEST_NAME "otp2"
#include "cmptest.h"

#include "crypto_otp.h"
#include <stdio.h>

int main(void)
{
    const char key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10 };
    const uint64_t initialTime = 0ul;
    const uint64_t step = 30ul;
    const uint64_t now = 1457533659ul + 90ul;
    const unsigned int digits = 6;
    const unsigned int window = 2;
    
    const char* tokens_sha256[] = {
        "175860",
        "707630",
        "075807",
        "281688", // Now
        "528562",
        "967403",
        "688930"
    };

    const char* tokens_sha512[] = {
        "436564",
        "335448",
        "327825",
        "244619", // Now
        "962706",
        "600145",
        "553734"
    };

    int i;
    for (i = 0; i < 7; i++)
        printf("%d\n", crypto_totp_sha256_verify(key, 10, now, initialTime, step, digits, window, tokens_sha256[i]));

    for (i = 0; i < 7; i++)
        printf("%d\n", crypto_totp_sha512_verify(key, 10, now, initialTime, step, digits, window, tokens_sha512[i]));

    return 0;
}
