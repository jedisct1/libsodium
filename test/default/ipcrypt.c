#define TEST_NAME "ipcrypt"
#include "cmptest.h"

static const unsigned char ipv4_mapped_prefix[12] = { 0U, 0U, 0U, 0U, 0U,    0U,
                                                      0U, 0U, 0U, 0U, 0xffU, 0xffU };

static int
check_expected(const char *test_name, const unsigned char *actual, const char *expected_hex,
               size_t len)
{
    unsigned char expected[64];
    char          actual_hex[129];

    sodium_hex2bin(expected, sizeof expected, expected_hex, strlen(expected_hex), NULL, NULL, NULL);
    if (memcmp(actual, expected, len) != 0) {
        sodium_bin2hex(actual_hex, sizeof actual_hex, actual, len);
        printf("FAILED %s: expected %s, got %s\n", test_name, expected_hex, actual_hex);
        return 1;
    }
    printf("OK: %s\n", test_name);
    return 0;
}

static void
flip_bit_msb(unsigned char buf[16], unsigned int bit_index)
{
    unsigned int byte_index;
    unsigned int bit_in_byte;

    if (bit_index >= 128U) {
        return;
    }
    byte_index  = bit_index / 8U;
    bit_in_byte = 7U - (bit_index % 8U);
    buf[byte_index] ^= (unsigned char) (1U << bit_in_byte);
}

static int
prefix_equal_msb(const unsigned char *a, const unsigned char *b, unsigned int prefix_len_bits)
{
    unsigned int full_bytes = prefix_len_bits / 8U;
    unsigned int rem_bits   = prefix_len_bits % 8U;

    if (prefix_len_bits == 0U) {
        return 1;
    }
    if (memcmp(a, b, full_bytes) != 0) {
        return 0;
    }
    if (rem_bits == 0U) {
        return 1;
    }
    return (a[full_bytes] & (unsigned char) (0xffU << (8U - rem_bits))) ==
           (b[full_bytes] & (unsigned char) (0xffU << (8U - rem_bits)));
}

int
main(void)
{
    unsigned char key[crypto_ipcrypt_KEYBYTES];
    unsigned char ndx_key[crypto_ipcrypt_NDX_KEYBYTES];
    unsigned char pfx_key[crypto_ipcrypt_PFX_KEYBYTES];
    unsigned char input[crypto_ipcrypt_BYTES];
    unsigned char output[crypto_ipcrypt_BYTES];
    unsigned char nd_output[crypto_ipcrypt_ND_OUTPUTBYTES];
    unsigned char nd_output2[crypto_ipcrypt_ND_OUTPUTBYTES];
    unsigned char ndx_output[crypto_ipcrypt_NDX_OUTPUTBYTES];
    unsigned char ndx_output2[crypto_ipcrypt_NDX_OUTPUTBYTES];
    unsigned char pfx_output[crypto_ipcrypt_PFX_BYTES];
    unsigned char tweak_nd[crypto_ipcrypt_ND_TWEAKBYTES];
    unsigned char tweak_nd2[crypto_ipcrypt_ND_TWEAKBYTES];
    unsigned char tweak_ndx[crypto_ipcrypt_NDX_TWEAKBYTES];
    unsigned char tweak_ndx2[crypto_ipcrypt_NDX_TWEAKBYTES];
    unsigned char decrypted[crypto_ipcrypt_BYTES];
    unsigned char encrypted1[crypto_ipcrypt_PFX_BYTES];
    unsigned char encrypted2[crypto_ipcrypt_PFX_BYTES];
    size_t        i;
    unsigned int  prefix_len_bits;

    printf("crypto_ipcrypt_BYTES: %zu\n", crypto_ipcrypt_bytes());
    printf("crypto_ipcrypt_KEYBYTES: %zu\n", crypto_ipcrypt_keybytes());
    printf("crypto_ipcrypt_ND_KEYBYTES: %zu\n", crypto_ipcrypt_nd_keybytes());
    printf("crypto_ipcrypt_ND_TWEAKBYTES: %zu\n", crypto_ipcrypt_nd_tweakbytes());
    printf("crypto_ipcrypt_ND_INPUTBYTES: %zu\n", crypto_ipcrypt_nd_inputbytes());
    printf("crypto_ipcrypt_ND_OUTPUTBYTES: %zu\n", crypto_ipcrypt_nd_outputbytes());
    printf("crypto_ipcrypt_NDX_KEYBYTES: %zu\n", crypto_ipcrypt_ndx_keybytes());
    printf("crypto_ipcrypt_NDX_TWEAKBYTES: %zu\n", crypto_ipcrypt_ndx_tweakbytes());
    printf("crypto_ipcrypt_NDX_INPUTBYTES: %zu\n", crypto_ipcrypt_ndx_inputbytes());
    printf("crypto_ipcrypt_NDX_OUTPUTBYTES: %zu\n", crypto_ipcrypt_ndx_outputbytes());
    printf("crypto_ipcrypt_PFX_KEYBYTES: %zu\n", crypto_ipcrypt_pfx_keybytes());
    printf("crypto_ipcrypt_PFX_BYTES: %zu\n", crypto_ipcrypt_pfx_bytes());

    printf("\nipcrypt-deterministic test vectors\n");

    sodium_hex2bin(key, sizeof key, "0123456789abcdeffedcba9876543210", 32, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;

    crypto_ipcrypt_encrypt(output, input, key);
    if (check_expected("deterministic vector 1 (0.0.0.0)", output,
                       "bde96789d353824cd7c6f58a6bd226eb", 16) != 0) {
        return 1;
    }
    crypto_ipcrypt_decrypt(decrypted, output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: deterministic vector 1 round-trip\n");
        return 1;
    }

    sodium_hex2bin(key, sizeof key, "1032547698badcfeefcdab8967452301", 32, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 0xff;
    input[13] = 0xff;
    input[14] = 0xff;
    input[15] = 0xff;

    crypto_ipcrypt_encrypt(output, input, key);
    if (check_expected("deterministic vector 2 (255.255.255.255)", output,
                       "aed292f6ea2358c348fd08b874e845d8", 16) != 0) {
        return 1;
    }
    crypto_ipcrypt_decrypt(decrypted, output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: deterministic vector 2 round-trip\n");
        return 1;
    }

    sodium_hex2bin(key, sizeof key, "2b7e151628aed2a6abf7158809cf4f3c", 32, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 192;
    input[13] = 0;
    input[14] = 2;
    input[15] = 1;

    crypto_ipcrypt_encrypt(output, input, key);
    if (check_expected("deterministic vector 3 (192.0.2.1)", output,
                       "1dbdc1b9fff175867d0b67b4e76e4777", 16) != 0) {
        return 1;
    }
    crypto_ipcrypt_decrypt(decrypted, output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: deterministic vector 3 round-trip\n");
        return 1;
    }

    printf("\nipcrypt-nd test vectors\n");

    sodium_hex2bin(key, sizeof key, "0123456789abcdeffedcba9876543210", 32, NULL, NULL, NULL);
    sodium_hex2bin(tweak_nd, sizeof tweak_nd, "08e0c289bff23b7c", 16, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;

    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    if (check_expected("nd vector 1 (0.0.0.0)", nd_output,
                       "08e0c289bff23b7cb349aadfe3bcef56221c384c7c217b16", 24) != 0) {
        return 1;
    }
    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: nd vector 1 round-trip\n");
        return 1;
    }

    sodium_hex2bin(key, sizeof key, "1032547698badcfeefcdab8967452301", 32, NULL, NULL, NULL);
    sodium_hex2bin(tweak_nd, sizeof tweak_nd, "21bd1834bc088cd2", 16, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 192;
    input[13] = 0;
    input[14] = 2;
    input[15] = 1;

    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    if (check_expected("nd vector 2 (192.0.2.1)", nd_output,
                       "21bd1834bc088cd2e5e1fe55f95876e639faae2594a0caad", 24) != 0) {
        return 1;
    }
    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: nd vector 2 round-trip\n");
        return 1;
    }

    sodium_hex2bin(key, sizeof key, "2b7e151628aed2a6abf7158809cf4f3c", 32, NULL, NULL, NULL);
    sodium_hex2bin(tweak_nd, sizeof tweak_nd, "b4ecbe30b70898d7", 16, NULL, NULL, NULL);
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[15] = 0x01;

    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    if (check_expected("nd vector 3 (2001:db8::1)", nd_output,
                       "b4ecbe30b70898d7553ac8974d1b4250eafc4b0aa1f80c96", 24) != 0) {
        return 1;
    }
    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: nd vector 3 round-trip\n");
        return 1;
    }

    printf("\nipcrypt-ndx test vectors\n");

    sodium_hex2bin(ndx_key, sizeof ndx_key,
                   "0123456789abcdeffedcba98765432101032547698badcfeefcdab8967452301", 64, NULL,
                   NULL, NULL);
    sodium_hex2bin(tweak_ndx, sizeof tweak_ndx, "21bd1834bc088cd2b4ecbe30b70898d7", 32, NULL, NULL,
                   NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;

    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    if (check_expected("ndx vector 1 (0.0.0.0)", ndx_output,
                       "21bd1834bc088cd2b4ecbe30b70898d782db0d4125fdace61db35b8339f20ee5",
                       32) != 0) {
        return 1;
    }
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx vector 1 round-trip\n");
        return 1;
    }

    sodium_hex2bin(ndx_key, sizeof ndx_key,
                   "1032547698badcfeefcdab89674523010123456789abcdeffedcba9876543210", 64, NULL,
                   NULL, NULL);
    sodium_hex2bin(tweak_ndx, sizeof tweak_ndx, "08e0c289bff23b7cb4ecbe30b70898d7", 32, NULL, NULL,
                   NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 192;
    input[13] = 0;
    input[14] = 2;
    input[15] = 1;

    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    if (check_expected("ndx vector 2 (192.0.2.1)", ndx_output,
                       "08e0c289bff23b7cb4ecbe30b70898d7766a533392a69edf1ad0d3ce362ba98a",
                       32) != 0) {
        return 1;
    }
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx vector 2 round-trip\n");
        return 1;
    }

    sodium_hex2bin(ndx_key, sizeof ndx_key,
                   "2b7e151628aed2a6abf7158809cf4f3c3c4fcf098815f7aba6d2ae2816157e2b", 64, NULL,
                   NULL, NULL);
    sodium_hex2bin(tweak_ndx, sizeof tweak_ndx, "21bd1834bc088cd2b4ecbe30b70898d7", 32, NULL, NULL,
                   NULL);
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[15] = 0x01;

    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    if (check_expected("ndx vector 3 (2001:db8::1)", ndx_output,
                       "21bd1834bc088cd2b4ecbe30b70898d76089c7e05ae30c2d10ca149870a263e4",
                       32) != 0) {
        return 1;
    }
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx vector 3 round-trip\n");
        return 1;
    }

    printf("\nipcrypt-pfx test vectors\n");

    sodium_hex2bin(pfx_key, sizeof pfx_key,
                   "0123456789abcdeffedcba98765432101032547698badcfeefcdab8967452301", 64, NULL,
                   NULL, NULL);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx vector 1 (0.0.0.0)", pfx_output, "00000000000000000000ffff97529b86",
                       16) != 0) {
        return 1;
    }
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx vector 1 round-trip\n");
        return 1;
    }

    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 255;
    input[13] = 255;
    input[14] = 255;
    input[15] = 255;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx vector 2 (255.255.255.255)", pfx_output,
                       "00000000000000000000ffff5eb9a959", 16) != 0) {
        return 1;
    }
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx vector 2 round-trip\n");
        return 1;
    }

    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 192;
    input[13] = 0;
    input[14] = 2;
    input[15] = 1;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx vector 3 (192.0.2.1)", pfx_output, "00000000000000000000ffff64734883",
                       16) != 0) {
        return 1;
    }
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx vector 3 round-trip\n");
        return 1;
    }

    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[15] = 0x01;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx vector 4 (2001:db8::1)", pfx_output, "c1805dd42587352430abfa656ab60f88",
                       16) != 0) {
        return 1;
    }
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx vector 4 round-trip\n");
        return 1;
    }

    printf("\nipcrypt-pfx prefix preservation\n");

    sodium_hex2bin(pfx_key, sizeof pfx_key,
                   "2b7e151628aed2a6abf7158809cf4f3ca9f5ba40db214c3798f2e1c23456789a", 64, NULL,
                   NULL, NULL);

    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 10;
    input[13] = 0;
    input[14] = 0;
    input[15] = 47;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx /24 test (10.0.0.47)", pfx_output, "00000000000000000000ffff13d6d2f4",
                       16) != 0) {
        return 1;
    }

    input[15] = 129;
    crypto_ipcrypt_pfx_encrypt(encrypted1, input, pfx_key);
    if (check_expected("pfx /24 test (10.0.0.129)", encrypted1, "00000000000000000000ffff13d6d250",
                       16) != 0) {
        return 1;
    }

    input[15] = 234;
    crypto_ipcrypt_pfx_encrypt(encrypted2, input, pfx_key);
    if (check_expected("pfx /24 test (10.0.0.234)", encrypted2, "00000000000000000000ffff13d6d21e",
                       16) != 0) {
        return 1;
    }

    if (memcmp(pfx_output + 12, encrypted1 + 12, 3) != 0 ||
        memcmp(pfx_output + 12, encrypted2 + 12, 3) != 0) {
        printf("FAILED: IPv4 /24 prefix not preserved\n");
        return 1;
    }
    printf("OK: IPv4 /24 prefix preserved for 10.0.0.x\n");

    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 172;
    input[13] = 16;
    input[14] = 5;
    input[15] = 193;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx /16 test (172.16.5.193)", pfx_output,
                       "00000000000000000000ffffd24ee588", 16) != 0) {
        return 1;
    }

    input[14] = 97;
    input[15] = 42;
    crypto_ipcrypt_pfx_encrypt(encrypted1, input, pfx_key);
    if (check_expected("pfx /16 test (172.16.97.42)", encrypted1,
                       "00000000000000000000ffffd24eb3f1", 16) != 0) {
        return 1;
    }

    input[14] = 248;
    input[15] = 177;
    crypto_ipcrypt_pfx_encrypt(encrypted2, input, pfx_key);
    if (check_expected("pfx /16 test (172.16.248.177)", encrypted2,
                       "00000000000000000000ffffd24e79d7", 16) != 0) {
        return 1;
    }

    if (memcmp(pfx_output + 12, encrypted1 + 12, 2) != 0 ||
        memcmp(pfx_output + 12, encrypted2 + 12, 2) != 0) {
        printf("FAILED: IPv4 /16 prefix not preserved\n");
        return 1;
    }
    printf("OK: IPv4 /16 prefix preserved for 172.16.x.x\n");

    if (memcmp(pfx_output, ipv4_mapped_prefix, 12) != 0 ||
        memcmp(encrypted1, ipv4_mapped_prefix, 12) != 0 ||
        memcmp(encrypted2, ipv4_mapped_prefix, 12) != 0) {
        printf("FAILED: IPv4-mapped prefix not preserved in output\n");
        return 1;
    }
    printf("OK: IPv4-mapped prefix preserved in output\n");

    /* 2001:db8::a5c9:4e2f:bb91:5a7d */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[8]  = 0xa5;
    input[9]  = 0xc9;
    input[10] = 0x4e;
    input[11] = 0x2f;
    input[12] = 0xbb;
    input[13] = 0x91;
    input[14] = 0x5a;
    input[15] = 0x7d;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx /64 test (2001:db8::a5c9:4e2f:bb91:5a7d)", pfx_output,
                       "7cec702c12430f7019560125b9bd1aba", 16) != 0) {
        return 1;
    }

    /* 2001:db8::7234:d8f1:3c6e:9a52 */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[8]  = 0x72;
    input[9]  = 0x34;
    input[10] = 0xd8;
    input[11] = 0xf1;
    input[12] = 0x3c;
    input[13] = 0x6e;
    input[14] = 0x9a;
    input[15] = 0x52;

    crypto_ipcrypt_pfx_encrypt(encrypted1, input, pfx_key);
    if (check_expected("pfx /64 test (2001:db8::7234:d8f1:3c6e:9a52)", encrypted1,
                       "7cec702c12430f70a3ef0c8e95c1cd0d", 16) != 0) {
        return 1;
    }

    /* 2001:db8::f1e0:937b:26d4:8c1a */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[8]  = 0xf1;
    input[9]  = 0xe0;
    input[10] = 0x93;
    input[11] = 0x7b;
    input[12] = 0x26;
    input[13] = 0xd4;
    input[14] = 0x8c;
    input[15] = 0x1a;

    crypto_ipcrypt_pfx_encrypt(encrypted2, input, pfx_key);
    if (check_expected("pfx /64 test (2001:db8::f1e0:937b:26d4:8c1a)", encrypted2,
                       "7cec702c12430f70443c0c8e6a62b64d", 16) != 0) {
        return 1;
    }

    if (memcmp(pfx_output, encrypted1, 8) != 0 || memcmp(pfx_output, encrypted2, 8) != 0) {
        printf("FAILED: IPv6 /64 prefix not preserved\n");
        return 1;
    }
    printf("OK: IPv6 /64 prefix preserved for 2001:db8::/64\n");

    /* 2001:db8:3a5c:0:e7d1:4b9f:2c8a:f673 */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[4]  = 0x3a;
    input[5]  = 0x5c;
    input[8]  = 0xe7;
    input[9]  = 0xd1;
    input[10] = 0x4b;
    input[11] = 0x9f;
    input[12] = 0x2c;
    input[13] = 0x8a;
    input[14] = 0xf6;
    input[15] = 0x73;

    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    if (check_expected("pfx /32 test (2001:db8:3a5c:0:e7d1:4b9f:2c8a:f673)", pfx_output,
                       "7cec702c35030befe61696bdbe33a9b9", 16) != 0) {
        return 1;
    }

    /* 2001:db8:9f27:0:b4e2:7a3d:5f91:c8e6 */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[4]  = 0x9f;
    input[5]  = 0x27;
    input[8]  = 0xb4;
    input[9]  = 0xe2;
    input[10] = 0x7a;
    input[11] = 0x3d;
    input[12] = 0x5f;
    input[13] = 0x91;
    input[14] = 0xc8;
    input[15] = 0xe6;

    crypto_ipcrypt_pfx_encrypt(encrypted1, input, pfx_key);
    if (check_expected("pfx /32 test (2001:db8:9f27:0:b4e2:7a3d:5f91:c8e6)", encrypted1,
                       "7cec702ca504b74e194a3d90b0472d1a", 16) != 0) {
        return 1;
    }

    /* 2001:db8:d8b4:0:193c:a5e7:8b2f:46d1 */
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[4]  = 0xd8;
    input[5]  = 0xb4;
    input[8]  = 0x19;
    input[9]  = 0x3c;
    input[10] = 0xa5;
    input[11] = 0xe7;
    input[12] = 0x8b;
    input[13] = 0x2f;
    input[14] = 0x46;
    input[15] = 0xd1;

    crypto_ipcrypt_pfx_encrypt(encrypted2, input, pfx_key);
    if (check_expected("pfx /32 test (2001:db8:d8b4:0:193c:a5e7:8b2f:46d1)", encrypted2,
                       "7cec702cf840aa6701b8e84fac9d77fb", 16) != 0) {
        return 1;
    }

    if (memcmp(pfx_output, encrypted1, 4) != 0 || memcmp(pfx_output, encrypted2, 4) != 0) {
        printf("FAILED: IPv6 /32 prefix not preserved\n");
        return 1;
    }
    printf("OK: IPv6 /32 prefix preserved for 2001:db8::/32\n");

    printf("\nsystematic prefix-length coverage\n");

    memset(pfx_key, 0x6a, sizeof pfx_key);
    for (prefix_len_bits = 0; prefix_len_bits <= 128U; prefix_len_bits++) {
        unsigned char in1[16];
        unsigned char in2[16];
        unsigned char out1[16];
        unsigned char out2[16];

        randombytes_buf(in1, sizeof in1);
        in1[0] = 0x20;
        in1[1] = 0x01;
        memcpy(in2, in1, sizeof in1);
        flip_bit_msb(in2, prefix_len_bits);
        crypto_ipcrypt_pfx_encrypt(out1, in1, pfx_key);
        crypto_ipcrypt_pfx_encrypt(out2, in2, pfx_key);
        if (prefix_equal_msb(out1, out2, prefix_len_bits) == 0) {
            printf("FAILED: IPv6 prefix length %u\n", prefix_len_bits);
            return 1;
        }
    }
    printf("OK: IPv6 prefix preservation for /0..128\n");

    for (prefix_len_bits = 0; prefix_len_bits <= 32U; prefix_len_bits++) {
        unsigned char in1[16];
        unsigned char in2[16];
        unsigned char out1[16];
        unsigned char out2[16];

        memset(in1, 0, sizeof in1);
        memcpy(in1, ipv4_mapped_prefix, 12);
        randombytes_buf(in1 + 12, 4U);
        memcpy(in2, in1, sizeof in1);
        flip_bit_msb(in2, 96U + prefix_len_bits);
        crypto_ipcrypt_pfx_encrypt(out1, in1, pfx_key);
        crypto_ipcrypt_pfx_encrypt(out2, in2, pfx_key);
        if (prefix_equal_msb(out1, out2, 96U + prefix_len_bits) == 0) {
            printf("FAILED: IPv4-mapped prefix length %u\n", prefix_len_bits);
            return 1;
        }
    }
    printf("OK: IPv4-mapped prefix preservation for /0..32\n");

    printf("\nfunctional tests\n");

    crypto_ipcrypt_keygen(key);
    crypto_ipcrypt_ndx_keygen(ndx_key);
    crypto_ipcrypt_pfx_keygen(pfx_key);
    printf("OK: Key generation functions\n");

    memset(key, 0x55, sizeof key);
    memset(input, 0xaa, sizeof input);
    crypto_ipcrypt_encrypt(output, input, key);
    memcpy(decrypted, input, sizeof input);
    crypto_ipcrypt_encrypt(decrypted, decrypted, key);
    if (memcmp(output, decrypted, sizeof output) != 0) {
        printf("FAILED: In-place encryption differs\n");
        return 1;
    }
    crypto_ipcrypt_decrypt(decrypted, decrypted, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: In-place round-trip\n");
        return 1;
    }
    printf("OK: In-place encryption/decryption\n");

    crypto_ipcrypt_encrypt(output, input, key);
    crypto_ipcrypt_encrypt(decrypted, input, key);
    if (memcmp(output, decrypted, sizeof output) != 0) {
        printf("FAILED: Deterministic encryption\n");
        return 1;
    }
    printf("OK: Deterministic encryption\n");

    memset(key, 0x42, sizeof key);
    for (i = 0; i < 4; i++) {
        memset(input, 0, sizeof input);
        input[10] = 0xff;
        input[11] = 0xff;
        input[15] = (unsigned char) i;
        crypto_ipcrypt_encrypt(output, input, key);
        if (i > 0) {
            if (memcmp(output, decrypted, sizeof output) == 0) {
                printf("FAILED: Different inputs produced same output\n");
                return 1;
            }
        }
        memcpy(decrypted, output, sizeof output);
    }
    printf("OK: Different inputs produce different outputs\n");

    printf("\nedge case tests\n");

    memset(key, 0x11, sizeof key);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    memset(tweak_nd, 0, sizeof tweak_nd);
    memset(tweak_nd2, 0, sizeof tweak_nd2);
    tweak_nd2[sizeof tweak_nd2 - 1] = 1U;
    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    crypto_ipcrypt_nd_encrypt(nd_output2, input, tweak_nd2, key);
    if (memcmp(nd_output, nd_output2, sizeof nd_output) == 0) {
        printf("FAILED: nd tweak change did not affect output\n");
        return 1;
    }
    printf("OK: nd tweak changes output\n");

    memset(ndx_key, 0x22, 16);
    memset(ndx_key + 16, 0x33, 16);
    memset(tweak_ndx, 0, sizeof tweak_ndx);
    memset(tweak_ndx2, 0, sizeof tweak_ndx2);
    tweak_ndx2[0] = 1U;
    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    crypto_ipcrypt_ndx_encrypt(ndx_output2, input, tweak_ndx2, ndx_key);
    if (memcmp(ndx_output, ndx_output2, sizeof ndx_output) == 0) {
        printf("FAILED: ndx tweak change did not affect output\n");
        return 1;
    }
    printf("OK: ndx tweak changes output\n");

    memset(ndx_key, 0x44, 16);
    memcpy(ndx_key + 16, ndx_key, 16);
    memset(input, 0xaa, sizeof input);
    memset(tweak_ndx, 0x5a, sizeof tweak_ndx);
    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx equal-halves key round-trip\n");
        return 1;
    }
    printf("OK: ndx equal-halves key round-trip\n");

    memset(pfx_key, 0x5b, 16);
    memcpy(pfx_key + 16, pfx_key, 16);
    memset(input, 0, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 203;
    input[13] = 0;
    input[14] = 113;
    input[15] = 7;
    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx equal-halves key round-trip\n");
        return 1;
    }
    printf("OK: pfx equal-halves key round-trip\n");

    memset(pfx_key, 0x7a, sizeof pfx_key);
    memset(input, 0, sizeof input);
    input[0]  = 0x20;
    input[1]  = 0x01;
    input[2]  = 0x0d;
    input[3]  = 0xb8;
    input[15] = 0x01;
    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    memcpy(decrypted, input, sizeof input);
    crypto_ipcrypt_pfx_encrypt(decrypted, decrypted, pfx_key);
    if (memcmp(pfx_output, decrypted, sizeof pfx_output) != 0) {
        printf("FAILED: pfx in-place encryption differs\n");
        return 1;
    }
    crypto_ipcrypt_pfx_decrypt(decrypted, decrypted, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx in-place round-trip\n");
        return 1;
    }
    printf("OK: pfx in-place encryption/decryption\n");

    memset(key, 0x13, sizeof key);
    memset(ndx_key, 0x37, sizeof ndx_key);
    memset(pfx_key, 0x59, sizeof pfx_key);
    memset(tweak_nd, 0x1a, sizeof tweak_nd);
    memset(tweak_ndx, 0x2b, sizeof tweak_ndx);

    memset(input, 0, sizeof input);
    crypto_ipcrypt_encrypt(output, input, key);
    crypto_ipcrypt_decrypt(decrypted, output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ipcrypt IPv6 :: round-trip\n");
        return 1;
    }
    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: nd IPv6 :: round-trip\n");
        return 1;
    }
    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx IPv6 :: round-trip\n");
        return 1;
    }
    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx IPv6 :: round-trip\n");
        return 1;
    }
    printf("OK: IPv6 :: round-trips\n");

    memset(input, 0xff, sizeof input);
    crypto_ipcrypt_encrypt(output, input, key);
    crypto_ipcrypt_decrypt(decrypted, output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ipcrypt IPv6 all-ones round-trip\n");
        return 1;
    }
    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: nd IPv6 all-ones round-trip\n");
        return 1;
    }
    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ndx IPv6 all-ones round-trip\n");
        return 1;
    }
    crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
    crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: pfx IPv6 all-ones round-trip\n");
        return 1;
    }
    printf("OK: IPv6 all-ones round-trips\n");

    printf("\nrandomized property tests\n");

    for (i = 0; i < 128U; i++) {
        randombytes_buf(key, sizeof key);
        randombytes_buf(ndx_key, sizeof ndx_key);
        randombytes_buf(pfx_key, sizeof pfx_key);
        randombytes_buf(tweak_nd, sizeof tweak_nd);
        randombytes_buf(tweak_ndx, sizeof tweak_ndx);
        randombytes_buf(input, sizeof input);

        crypto_ipcrypt_encrypt(output, input, key);
        crypto_ipcrypt_decrypt(decrypted, output, key);
        if (memcmp(input, decrypted, sizeof input) != 0) {
            printf("FAILED: randomized ipcrypt round-trip\n");
            return 1;
        }

        crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
        crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
        if (memcmp(input, decrypted, sizeof input) != 0) {
            printf("FAILED: randomized nd round-trip\n");
            return 1;
        }

        crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
        crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
        if (memcmp(input, decrypted, sizeof input) != 0) {
            printf("FAILED: randomized ndx round-trip\n");
            return 1;
        }

        crypto_ipcrypt_pfx_encrypt(pfx_output, input, pfx_key);
        crypto_ipcrypt_pfx_decrypt(decrypted, pfx_output, pfx_key);
        if (memcmp(input, decrypted, sizeof input) != 0) {
            printf("FAILED: randomized pfx round-trip\n");
            return 1;
        }
    }
    printf("OK: randomized round-trip tests\n");

    for (i = 0; i < 128U; i++) {
        unsigned char in1[16];
        unsigned char in2[16];
        unsigned char out1[16];
        unsigned char out2[16];

        randombytes_buf(pfx_key, sizeof pfx_key);
        randombytes_buf(in1, sizeof in1);
        in1[0] = 0x20;
        in1[1] = 0x01;
        memcpy(in2, in1, sizeof in1);
        prefix_len_bits = (unsigned int) randombytes_uniform(129U);
        flip_bit_msb(in2, prefix_len_bits);
        crypto_ipcrypt_pfx_encrypt(out1, in1, pfx_key);
        crypto_ipcrypt_pfx_encrypt(out2, in2, pfx_key);
        if (prefix_equal_msb(out1, out2, prefix_len_bits) == 0) {
            printf("FAILED: randomized IPv6 prefix length %u\n", prefix_len_bits);
            return 1;
        }
    }
    printf("OK: randomized IPv6 prefix preservation\n");

    for (i = 0; i < 128U; i++) {
        unsigned char in1[16];
        unsigned char in2[16];
        unsigned char out1[16];
        unsigned char out2[16];

        randombytes_buf(pfx_key, sizeof pfx_key);
        memset(in1, 0, sizeof in1);
        memcpy(in1, ipv4_mapped_prefix, 12);
        randombytes_buf(in1 + 12, 4U);
        memcpy(in2, in1, sizeof in1);
        prefix_len_bits = (unsigned int) randombytes_uniform(33U);
        flip_bit_msb(in2, 96U + prefix_len_bits);
        crypto_ipcrypt_pfx_encrypt(out1, in1, pfx_key);
        crypto_ipcrypt_pfx_encrypt(out2, in2, pfx_key);
        if (prefix_equal_msb(out1, out2, 96U + prefix_len_bits) == 0) {
            printf("FAILED: randomized IPv4-mapped prefix length %u\n", prefix_len_bits);
            return 1;
        }
    }
    printf("OK: randomized IPv4-mapped prefix preservation\n");

    printf("\nAll specification test vectors passed!\n");

    return 0;
}
