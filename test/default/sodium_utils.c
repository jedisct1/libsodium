#define TEST_NAME "sodium_utils"
#include "cmptest.h"

int
main(void)
{
    unsigned char buf_add[1000];
    unsigned char buf1[1000];
    unsigned char buf2[1000];
    unsigned char buf1_rev[1000];
    unsigned char buf2_rev[1000];
    char          buf3[33];
    unsigned char buf4[4];
    unsigned char nonce[24];
    char          nonce_hex[49];
    const char *  hex;
    const char *  hex_end;
    size_t        bin_len;
    unsigned int  i;
    unsigned int  j;

    randombytes_buf(buf1, sizeof buf1);
    memcpy(buf2, buf1, sizeof buf2);
    printf("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
    sodium_memzero(buf1, 0U);
    printf("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
    sodium_memzero(buf1, sizeof buf1 / 2);
    printf("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
    printf("%d\n", sodium_memcmp(buf1, buf2, 0U));
    sodium_memzero(buf2, sizeof buf2 / 2);
    printf("%d\n", sodium_memcmp(buf1, buf2, sizeof buf1));
    printf("%s\n",
           sodium_bin2hex(buf3, 33U, (const unsigned char *) "0123456789ABCDEF",
                          16U));
    hex = "Cafe : 6942";
    sodium_hex2bin(buf4, sizeof buf4, hex, strlen(hex), ": ", &bin_len,
                   &hex_end);
    printf("%lu:%02x%02x%02x%02x\n", (unsigned long) bin_len, buf4[0], buf4[1],
           buf4[2], buf4[3]);
    printf("dt1: %ld\n", (long) (hex_end - hex));

    hex = "Cafe : 6942";
    sodium_hex2bin(buf4, sizeof buf4, hex, strlen(hex), ": ", &bin_len, NULL);
    printf("%lu:%02x%02x%02x%02x\n", (unsigned long) bin_len, buf4[2], buf4[3],
           buf4[2], buf4[3]);

    hex = "deadbeef";
    if (sodium_hex2bin(buf1, 1U, hex, 8U, NULL, &bin_len, &hex_end) != -1) {
        printf("sodium_hex2bin() overflow not detected\n");
    }
    printf("dt2: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:eff";
    if (sodium_hex2bin(buf1, 4U, hex, 12U, ":", &bin_len, &hex_end) != -1) {
        printf(
            "sodium_hex2bin() with an odd input length and a short output "
            "buffer\n");
    }
    printf("dt3: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:eff";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 12U, ":", &bin_len, &hex_end) !=
        0) {
        printf("sodium_hex2bin() with an odd input length\n");
    }
    printf("dt4: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:eff";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 13U, ":", &bin_len, &hex_end) !=
        0) {
        printf("sodium_hex2bin() with an odd input length\n");
    }
    printf("dt5: %ld\n", (long) (hex_end - hex));

    memset(nonce, 0, sizeof nonce);
    sodium_increment(nonce, sizeof nonce);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    memset(nonce, 255, sizeof nonce);
    sodium_increment(nonce, sizeof nonce);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    nonce[1] = 1U;
    sodium_increment(nonce, sizeof nonce);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    nonce[1] = 0U;
    sodium_increment(nonce, sizeof nonce);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    nonce[0] = 255U;
    nonce[2] = 255U;
    sodium_increment(nonce, sizeof nonce);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    for (i = 0U; i < 1000U; i++) {
        bin_len = (size_t) randombytes_uniform(sizeof buf1);
        randombytes_buf(buf1, bin_len);
        randombytes_buf(buf2, bin_len);
        for (j = 0U; j < bin_len; j++) {
            buf1_rev[bin_len - 1 - j] = buf1[j];
            buf2_rev[bin_len - 1 - j] = buf2[j];
        }
        if (memcmp(buf1_rev, buf2_rev, bin_len) *
                sodium_compare(buf1, buf2, bin_len) <
            0) {
            printf("sodium_compare() failure with length=%u\n",
                   (unsigned int) bin_len);
        }
        memcpy(buf1, buf2, bin_len);
        if (sodium_compare(buf1, buf2, bin_len)) {
            printf("sodium_compare() equality failure with length=%u\n",
                   (unsigned int) bin_len);
        }
    }
    memset(buf1, 0, sizeof buf1);
    if (sodium_is_zero(buf1, sizeof buf1) != 1) {
        printf("sodium_is_zero() failed\n");
    }
    for (i = 0U; i < sizeof buf1; i++) {
        buf1[i]++;
        if (sodium_is_zero(buf1, sizeof buf1) != 0) {
            printf("sodium_is_zero() failed\n");
        }
        buf1[i]--;
    }
    bin_len = randombytes_uniform(sizeof buf1);
    randombytes_buf(buf1, bin_len);
    memcpy(buf2, buf1, bin_len);
    memset(buf_add, 0, bin_len);
    j = randombytes_uniform(10000);
    for (i = 0U; i < j; i++) {
        sodium_increment(buf1, bin_len);
        sodium_increment(buf_add, bin_len);
    }
    sodium_add(buf2, buf_add, bin_len);
    if (sodium_compare(buf1, buf2, bin_len) != 0) {
        printf("sodium_add() failed\n");
    }
    bin_len = randombytes_uniform(sizeof buf1);
    randombytes_buf(buf1, bin_len);
    memcpy(buf2, buf1, bin_len);
    memset(buf_add, 0xff, bin_len);
    sodium_increment(buf2, bin_len);
    sodium_increment(buf2, 0U);
    sodium_add(buf2, buf_add, bin_len);
    sodium_add(buf2, buf_add, 0U);
    if (sodium_compare(buf1, buf2, bin_len) != 0) {
        printf("sodium_add() failed\n");
    }

    assert(sizeof nonce >= 24U);
    memset(nonce, 0xfe, 24U);
    memset(nonce, 0xff, 6U);
    sodium_increment(nonce, 8U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    memset(nonce, 0xfe, 24U);
    memset(nonce, 0xff, 10U);
    sodium_increment(nonce, 12U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    memset(nonce, 0xff, 22U);
    sodium_increment(nonce, 24U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));

    assert(sizeof nonce >= 24U);
    memset(nonce, 0xfe, 24U);
    memset(nonce, 0xff, 6U);
    sodium_add(nonce, nonce, 7U);
    sodium_add(nonce, nonce, 8U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    memset(nonce, 0xfe, 24U);
    memset(nonce, 0xff, 10U);
    sodium_add(nonce, nonce, 11U);
    sodium_add(nonce, nonce, 12U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));
    memset(nonce, 0xff, 22U);
    sodium_add(nonce, nonce, 23U);
    sodium_add(nonce, nonce, 24U);
    printf("%s\n",
           sodium_bin2hex(nonce_hex, sizeof nonce_hex, nonce, sizeof nonce));

    return 0;
}
