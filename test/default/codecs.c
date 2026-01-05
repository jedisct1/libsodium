#define TEST_NAME "codecs"
#include "cmptest.h"

int
main(void)
{
    unsigned char  buf1[1000];
    char           buf3[33];
    unsigned char  buf4[4];
    const char    *b64;
    char          *b64_;
    const char    *b64_end;
    unsigned char *bin;
    const char    *hex;
    const char    *hex_end;
    size_t         b64_len;
    size_t         bin_len;
    unsigned int   i;

    printf("%s\n",
           sodium_bin2hex(buf3, 33U, (const unsigned char *) "0123456789ABCDEF",
                          16U));
    printf("bin2hex(..., guard_page, 0):%s\n",
           sodium_bin2hex(buf3, sizeof buf3, guard_page, 0U));
    printf("bin2hex(..., \"\", 0):%s\n",
           sodium_bin2hex(buf3, sizeof buf3, (const unsigned char *) "", 0U));

    hex = "Cafe : 6942";
    sodium_hex2bin(buf4, sizeof buf4, hex, strlen(hex), ": ", &bin_len,
                   &hex_end);
    printf("%lu:%02x%02x%02x%02x\n", (unsigned long) bin_len,
           buf4[0], buf4[1], buf4[2], buf4[3]);
    printf("dt1: %ld\n", (long) (hex_end - hex));

    hex = "Cafe : 6942";
    sodium_hex2bin(buf4, sizeof buf4, hex, strlen(hex), ": ", &bin_len, NULL);
    printf("%lu:%02x%02x%02x%02x\n", (unsigned long) bin_len,
           buf4[0], buf4[1], buf4[2], buf4[3]);

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
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 12U, ":",
                       &bin_len, &hex_end) != -1) {
        printf("sodium_hex2bin() with an odd input length\n");
    }
    printf("dt4: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:eff";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 13U, ":",
                       &bin_len, &hex_end) != -1) {
        printf("sodium_hex2bin() with an odd input length (2)\n");
    }
    printf("dt5: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:eff";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 12U, ":",
                       &bin_len, NULL) != -1) {
        printf("sodium_hex2bin() with an odd input length and no end pointer\n");
    }

    hex = "de:ad:be:ef*";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 12U, ":",
                       &bin_len, &hex_end) != 0) {
        printf("sodium_hex2bin() with an extra character and an end pointer\n");
    }
    printf("dt6: %ld\n", (long) (hex_end - hex));

    hex = "de:ad:be:ef*";
    if (sodium_hex2bin(buf1, sizeof buf1, hex, 12U, ":",
                       &bin_len, NULL) != -1) {
        printf("sodium_hex2bin() with an extra character and no end pointer\n");
    }

    assert(sodium_hex2bin(buf4, sizeof buf4, (const char *) guard_page, 0U,
                          NULL, &bin_len, NULL) == 0);
    assert(bin_len == 0);

    assert(sodium_hex2bin(buf4, sizeof buf4, "", 0U, NULL, &bin_len, NULL) == 0);
    assert(bin_len == 0);

    printf("%s\n",
           sodium_bin2base64(buf3, 31U, (const unsigned char *) "\xfb\xf0\xf1" "0123456789ABCDEFab",
                             21U, sodium_base64_VARIANT_ORIGINAL));
    printf("%s\n",
           sodium_bin2base64(buf3, 33U, (const unsigned char *) "\xfb\xf0\xf1" "0123456789ABCDEFabc",
                             22U, sodium_base64_VARIANT_ORIGINAL_NO_PADDING));
    printf("%s\n",
           sodium_bin2base64(buf3, 31U, (const unsigned char *) "\xfb\xf0\xf1" "0123456789ABCDEFab",
                             21U, sodium_base64_VARIANT_URLSAFE));
    printf("%s\n",
           sodium_bin2base64(buf3, 33U, (const unsigned char *) "\xfb\xf0\xf1" "0123456789ABCDEFabc",
                             22U, sodium_base64_VARIANT_URLSAFE_NO_PADDING));
    printf("%s\n",
           sodium_bin2base64(buf3, 1U, guard_page,
                             0U, sodium_base64_VARIANT_ORIGINAL));
    printf("%s\n",
           sodium_bin2base64(buf3, 5U, (const unsigned char *) "a",
                             1U, sodium_base64_VARIANT_ORIGINAL));
    printf("%s\n",
           sodium_bin2base64(buf3, 5U, (const unsigned char *) "ab",
                             2U, sodium_base64_VARIANT_ORIGINAL));
    printf("%s\n",
           sodium_bin2base64(buf3, 5U, (const unsigned char *) "abc",
                             3U, sodium_base64_VARIANT_ORIGINAL));
    printf("%s\n",
           sodium_bin2base64(buf3, 1U, guard_page,
                             0U, sodium_base64_VARIANT_ORIGINAL_NO_PADDING));
    printf("%s\n",
           sodium_bin2base64(buf3, 3U, (const unsigned char *) "a",
                             1U, sodium_base64_VARIANT_ORIGINAL_NO_PADDING));
    printf("%s\n",
           sodium_bin2base64(buf3, 4U, (const unsigned char *) "ab",
                             2U, sodium_base64_VARIANT_ORIGINAL_NO_PADDING));
    printf("%s\n",
           sodium_bin2base64(buf3, 5U, (const unsigned char *) "abc",
                             3U, sodium_base64_VARIANT_ORIGINAL_NO_PADDING));

    b64 = "VGhpcyBpcyBhIGpvdXJu" "\n" "ZXkgaW50by" " " "Bzb3VuZA==";
    memset(buf4, '*', sizeof buf4);
    assert(sodium_base642bin(buf4, sizeof buf4, b64, strlen(b64), "\n\r ", &bin_len,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL) == -1);
    buf4[bin_len] = 0;
    printf("[%s]\n", (const char *) buf4);
    printf("[%s]\n", b64_end);

    memset(buf1, '*', sizeof buf1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), "\n\r ", &bin_len,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL) == 0);
    buf1[bin_len] = 0;
    printf("[%s]\n", (const char *) buf1);
    assert(*b64_end == 0);

    memset(buf1, '*', sizeof buf1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, &bin_len,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL) == 0);
    buf1[bin_len] = 0;
    printf("[%s]\n", (const char *) buf1);
    printf("[%s]\n", b64_end);

    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), " \r\n", NULL,
                             &b64_end, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             &b64_end, sodium_base64_VARIANT_URLSAFE_NO_PADDING) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), " \r\n", NULL,
                             &b64_end, sodium_base64_VARIANT_URLSAFE_NO_PADDING) == 0);

    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             NULL, sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             NULL, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), " \r\n", NULL,
                             NULL, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), NULL, NULL,
                             NULL, sodium_base64_VARIANT_URLSAFE_NO_PADDING) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, b64, strlen(b64), " \r\n", NULL,
                             NULL, sodium_base64_VARIANT_URLSAFE_NO_PADDING) == -1);

    assert(sodium_base642bin(guard_page, (size_t) 10U, "a=", (size_t) 2U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a*", (size_t) 2U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a*", (size_t) 2U, "~", NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a*", (size_t) 2U, "*", NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a==", (size_t) 3U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a=*", (size_t) 3U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a=*", (size_t) 3U, "~", NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);
    assert(sodium_base642bin(guard_page, (size_t) 10U, "a=*", (size_t) 3U, "*", NULL, NULL,
                             sodium_base64_VARIANT_URLSAFE) == -1);

    assert(sodium_base642bin(buf1, sizeof buf1, "O1R", (size_t) 3U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "O1Q", (size_t) 3U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, "O1", (size_t) 2U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "Ow", (size_t) 2U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, "O", (size_t) 1U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL_NO_PADDING) == -1);

    assert(sodium_base642bin(buf1, sizeof buf1, "", (size_t) 0U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, "A", (size_t) 1U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "AA", (size_t) 2U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "kaw", (size_t) 3U, NULL, NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "kQ*", (size_t) 3U, "@", NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "kQ*", (size_t) 3U, "*", NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == -1);
    assert(sodium_base642bin(buf1, sizeof buf1, "kaw=**", (size_t) 6U, "*", NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, "kaw*=*", (size_t) 6U, "~*", NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(sodium_base642bin(buf1, sizeof buf1, "ka*w*=*", (size_t) 7U, "*~", NULL, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == 0);

    assert(sodium_base642bin(buf1, sizeof buf1, (const char *) guard_page, 0U,
                             NULL, &bin_len, NULL, sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(bin_len == 0);

    assert(sodium_base642bin(buf1, sizeof buf1, "", 0U, NULL, &bin_len, NULL,
                             sodium_base64_VARIANT_ORIGINAL) == 0);
    assert(bin_len == 0);

    for (i = 0; i < 1000; i++) {
        assert(sizeof buf1 >= 100);
        bin_len = (size_t) randombytes_uniform(100);
        bin = (unsigned char *) sodium_malloc(bin_len);
        b64_len = (bin_len + 2U) / 3U * 4U + 1U;
        assert(b64_len == sodium_base64_encoded_len(bin_len, sodium_base64_VARIANT_URLSAFE));
        b64_ = (char *) sodium_malloc(b64_len);
        randombytes_buf(bin, bin_len);
        memcpy(buf1, bin, bin_len);
        b64 = sodium_bin2base64(b64_, b64_len, bin, bin_len,
                                sodium_base64_VARIANT_URLSAFE);
        assert(b64 != NULL);
        assert(sodium_base642bin(bin, bin_len + 10, b64, b64_len,
                                 NULL, NULL, &b64_end,
                                 sodium_base64_VARIANT_URLSAFE) == 0);
        assert(b64_end == &b64[b64_len - 1]);
        assert(memcmp(bin, buf1, bin_len) == 0);
        sodium_free(bin);
        sodium_free(b64_);
    }

    {
        unsigned char ip_bytes[16];
        unsigned char ip_expected[16];
        char          ip_str[46];

        assert(sodium_ip2bin(ip_bytes, "192.168.1.1", strlen("192.168.1.1")) == 0);
        memset(ip_expected, 0, 10);
        ip_expected[10] = 0xff;
        ip_expected[11] = 0xff;
        ip_expected[12] = 192;
        ip_expected[13] = 168;
        ip_expected[14] = 1;
        ip_expected[15] = 1;
        assert(memcmp(ip_bytes, ip_expected, 16) == 0);
        printf("ip2bytes(192.168.1.1): OK\n");

        assert(sodium_ip2bin(ip_bytes, "0.0.0.0", strlen("0.0.0.0")) == 0);
        assert(sodium_ip2bin(ip_bytes, "255.255.255.255", strlen("255.255.255.255")) == 0);
        assert(sodium_ip2bin(ip_bytes, "127.0.0.1", strlen("127.0.0.1")) == 0);
        printf("ip2bytes IPv4 basic: OK\n");

        assert(sodium_ip2bin(ip_bytes, "256.1.1.1", strlen("256.1.1.1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "1.999.1.1", strlen("1.999.1.1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "192.168.1", strlen("192.168.1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "192.168.1.1.1", strlen("192.168.1.1.1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "", 0) == -1);
        assert(sodium_ip2bin(ip_bytes, ".1.2.3", strlen(".1.2.3")) == -1);
        assert(sodium_ip2bin(ip_bytes, "1.2.3.", strlen("1.2.3.")) == -1);
        assert(sodium_ip2bin(ip_bytes, "1..2.3", strlen("1..2.3")) == -1);
        assert(sodium_ip2bin(ip_bytes, "1.2.a.3", strlen("1.2.a.3")) == -1);
        printf("ip2bytes IPv4 invalid: OK\n");

        assert(sodium_ip2bin(ip_bytes, "2001:db8:85a3:0:0:8a2e:370:7334", strlen("2001:db8:85a3:0:0:8a2e:370:7334")) == 0);
        ip_expected[0]  = 0x20; ip_expected[1]  = 0x01;
        ip_expected[2]  = 0x0d; ip_expected[3]  = 0xb8;
        ip_expected[4]  = 0x85; ip_expected[5]  = 0xa3;
        ip_expected[6]  = 0x00; ip_expected[7]  = 0x00;
        ip_expected[8]  = 0x00; ip_expected[9]  = 0x00;
        ip_expected[10] = 0x8a; ip_expected[11] = 0x2e;
        ip_expected[12] = 0x03; ip_expected[13] = 0x70;
        ip_expected[14] = 0x73; ip_expected[15] = 0x34;
        assert(memcmp(ip_bytes, ip_expected, 16) == 0);
        printf("ip2bytes(2001:db8:85a3:0:0:8a2e:370:7334): OK\n");

        assert(sodium_ip2bin(ip_bytes, "::1", strlen("::1")) == 0);
        memset(ip_expected, 0, 16);
        ip_expected[15] = 1;
        assert(memcmp(ip_bytes, ip_expected, 16) == 0);

        assert(sodium_ip2bin(ip_bytes, "::", strlen("::")) == 0);
        memset(ip_expected, 0, 16);
        assert(memcmp(ip_bytes, ip_expected, 16) == 0);

        assert(sodium_ip2bin(ip_bytes, "2001:db8::1", strlen("2001:db8::1")) == 0);
        assert(sodium_ip2bin(ip_bytes, "fe80::1", strlen("fe80::1")) == 0);
        printf("ip2bytes IPv6 compressed: OK\n");

        assert(sodium_ip2bin(ip_bytes, "fe80::1%eth0", strlen("fe80::1%eth0")) == 0);
        assert(sodium_ip2bin(ip_bytes, "fe80::1%15", strlen("fe80::1%15")) == 0);
        printf("ip2bytes IPv6 zone: OK\n");

        assert(sodium_ip2bin(ip_bytes, "2001:::1", strlen("2001:::1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "2001::1::1", strlen("2001::1::1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:8:9", strlen("1:2:3:4:5:6:7:8:9")) == -1);
        assert(sodium_ip2bin(ip_bytes, "12345:1:1:1:1:1:1:1", strlen("12345:1:1:1:1:1:1:1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "2001:db8:g:1:1:1:1:1", strlen("2001:db8:g:1:1:1:1:1")) == -1);
        assert(sodium_ip2bin(ip_bytes, ":2001:db8::1", strlen(":2001:db8::1")) == -1);
        assert(sodium_ip2bin(ip_bytes, "2001:db8:1:", strlen("2001:db8:1:")) == -1);
        assert(sodium_ip2bin(ip_bytes, "fe80::1%", strlen("fe80::1%")) == -1);
        printf("ip2bytes IPv6 invalid: OK\n");

        assert(sodium_ip2bin(ip_bytes, "::ffff:192.168.1.1", strlen("::ffff:192.168.1.1")) == 0);
        memset(ip_expected, 0, 10);
        ip_expected[10] = 0xff;
        ip_expected[11] = 0xff;
        ip_expected[12] = 192;
        ip_expected[13] = 168;
        ip_expected[14] = 1;
        ip_expected[15] = 1;
        assert(memcmp(ip_bytes, ip_expected, 16) == 0);
        printf("ip2bytes IPv4-mapped: OK\n");

        printf("ip NULL handling: OK\n");

        memset(ip_bytes, 0, 10);
        ip_bytes[10] = 0xff;
        ip_bytes[11] = 0xff;
        ip_bytes[12] = 192;
        ip_bytes[13] = 168;
        ip_bytes[14] = 1;
        ip_bytes[15] = 1;
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "192.168.1.1") == 0);

        ip_bytes[12] = 0;
        ip_bytes[13] = 0;
        ip_bytes[14] = 0;
        ip_bytes[15] = 0;
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "0.0.0.0") == 0);

        ip_bytes[12] = 255;
        ip_bytes[13] = 255;
        ip_bytes[14] = 255;
        ip_bytes[15] = 255;
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "255.255.255.255") == 0);
        printf("bytes2ip IPv4: OK\n");

        memset(ip_bytes, 0, 16);
        ip_bytes[15] = 1;
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "::1") == 0);

        memset(ip_bytes, 0, 16);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "::") == 0);

        memset(ip_bytes, 0xff, 16);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff") == 0);
        printf("bytes2ip IPv6: OK\n");

        memset(ip_bytes, 0, 10);
        ip_bytes[10] = 0xff;
        ip_bytes[11] = 0xff;
        ip_bytes[12] = 255;
        ip_bytes[13] = 255;
        ip_bytes[14] = 255;
        ip_bytes[15] = 255;
        assert(sodium_bin2ip(ip_str, 16, ip_bytes) != NULL);
        assert(sodium_bin2ip(ip_str, 15, ip_bytes) == NULL);

        memset(ip_bytes, 0xff, 16);
        assert(sodium_bin2ip(ip_str, 40, ip_bytes) != NULL);
        assert(sodium_bin2ip(ip_str, 39, ip_bytes) == NULL);

        memset(ip_bytes, 0, 16);
        assert(sodium_bin2ip(ip_str, 3, ip_bytes) != NULL);
        assert(sodium_bin2ip(ip_str, 2, ip_bytes) == NULL);
        printf("bytes2ip buffer size: OK\n");

        assert(sodium_ip2bin(ip_bytes, "10.20.30.40", strlen("10.20.30.40")) == 0);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "10.20.30.40") == 0);

        assert(sodium_ip2bin(ip_bytes, "::1", strlen("::1")) == 0);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "::1") == 0);

        assert(sodium_ip2bin(ip_bytes, "2001:db8::1", strlen("2001:db8::1")) == 0);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "2001:db8::1") == 0);

        assert(sodium_ip2bin(ip_bytes, "2001:db8:85a3:1234:5678:8a2e:370:7334", strlen("2001:db8:85a3:1234:5678:8a2e:370:7334")) == 0);
        assert(sodium_bin2ip(ip_str, sizeof(ip_str), ip_bytes) != NULL);
        assert(strcmp(ip_str, "2001:db8:85a3:1234:5678:8a2e:370:7334") == 0);
        printf("ip round-trip: OK\n");

        {
            unsigned char lower[16], upper[16], mixed[16];
            assert(sodium_ip2bin(lower, "abcd:ef01:2345:6789:abcd:ef01:2345:6789", strlen("abcd:ef01:2345:6789:abcd:ef01:2345:6789")) == 0);
            assert(sodium_ip2bin(upper, "ABCD:EF01:2345:6789:ABCD:EF01:2345:6789", strlen("ABCD:EF01:2345:6789:ABCD:EF01:2345:6789")) == 0);
            assert(sodium_ip2bin(mixed, "AbCd:eF01:2345:6789:aBcD:Ef01:2345:6789", strlen("AbCd:eF01:2345:6789:aBcD:Ef01:2345:6789")) == 0);
            assert(memcmp(lower, upper, 16) == 0);
            assert(memcmp(lower, mixed, 16) == 0);
        }
        printf("ip case insensitive: OK\n");

        {
            unsigned char direct[16], mapped[16];
            assert(sodium_ip2bin(direct, "192.168.1.1", strlen("192.168.1.1")) == 0);
            assert(sodium_ip2bin(mapped, "::ffff:192.168.1.1", strlen("::ffff:192.168.1.1")) == 0);
            assert(memcmp(direct, mapped, 16) == 0);
        }
        printf("ip IPv4-mapped equivalence: OK\n");

        {
            unsigned char ip1[16], ip2[16];

            assert(sodium_ip2bin(ip1, "192.168.1.1", 100) == 0);
            assert(sodium_ip2bin(ip2, "192.168.1.1", strlen("192.168.1.1")) == 0);
            assert(memcmp(ip1, ip2, 16) == 0);

            assert(sodium_ip2bin(ip1, "::1", 100) == 0);
            assert(sodium_ip2bin(ip2, "::1", strlen("::1")) == 0);
            assert(memcmp(ip1, ip2, 16) == 0);

            assert(sodium_ip2bin(ip1, "fe80::1%eth0", 100) == 0);
            assert(sodium_ip2bin(ip2, "fe80::1%eth0", strlen("fe80::1%eth0")) == 0);
            assert(memcmp(ip1, ip2, 16) == 0);
        }
        printf("ip src_len larger than string: OK\n");

        {
            unsigned char ip_bytes[16], ip_expected[16];

            assert(sodium_ip2bin(ip_bytes, "192.168.1.1", 3) == -1);
            assert(sodium_ip2bin(ip_bytes, "192.168.1.1", 10) == -1);
            assert(sodium_ip2bin(ip_bytes, "2001:db8::1", 5) == -1);

            assert(sodium_ip2bin(ip_bytes, "::1", 2) == 0);
            memset(ip_expected, 0, 16);
            assert(memcmp(ip_bytes, ip_expected, 16) == 0);
        }
        printf("ip src_len shorter than IP: OK\n");
    }

    return 0;
}
