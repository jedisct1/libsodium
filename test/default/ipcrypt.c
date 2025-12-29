#define TEST_NAME "ipcrypt"
#include "cmptest.h"

static void
dump_hex(const unsigned char *data, size_t len)
{
    char hex[129];

    sodium_bin2hex(hex, sizeof hex, data, len);
    printf("%s\n", hex);
}

int
main(void)
{
    unsigned char key[crypto_ipcrypt_KEYBYTES];
    unsigned char ndx_key[crypto_ipcrypt_NDX_KEYBYTES];
    unsigned char input[crypto_ipcrypt_BYTES];
    unsigned char output[crypto_ipcrypt_BYTES];
    unsigned char nd_output[crypto_ipcrypt_ND_OUTPUTBYTES];
    unsigned char ndx_output[crypto_ipcrypt_NDX_OUTPUTBYTES];
    unsigned char tweak_nd[crypto_ipcrypt_ND_TWEAKBYTES];
    unsigned char tweak_ndx[crypto_ipcrypt_NDX_TWEAKBYTES];
    unsigned char decrypted[crypto_ipcrypt_BYTES];
    size_t        i;

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

    /* Test 1: Format-preserving encryption with known key/input */
    memset(key, 0x00, sizeof key);
    key[0]  = 0x01;
    key[1]  = 0x02;
    key[2]  = 0x03;
    key[3]  = 0x04;
    key[4]  = 0x05;
    key[5]  = 0x06;
    key[6]  = 0x07;
    key[7]  = 0x08;
    key[8]  = 0x09;
    key[9]  = 0x0a;
    key[10] = 0x0b;
    key[11] = 0x0c;
    key[12] = 0x0d;
    key[13] = 0x0e;
    key[14] = 0x0f;
    key[15] = 0x10;

    /* IPv4-mapped IPv6 address: ::ffff:192.0.2.1 */
    memset(input, 0x00, sizeof input);
    input[10] = 0xff;
    input[11] = 0xff;
    input[12] = 0xc0; /* 192 */
    input[13] = 0x00; /* 0 */
    input[14] = 0x02; /* 2 */
    input[15] = 0x01; /* 1 */

    printf("\nTest 1: Format-preserving encryption\n");
    printf("Key: ");
    dump_hex(key, sizeof key);
    printf("Input: ");
    dump_hex(input, sizeof input);

    crypto_ipcrypt_encrypt(output, input, key);
    printf("Encrypted: ");
    dump_hex(output, sizeof output);

    crypto_ipcrypt_decrypt(decrypted, output, key);
    printf("Decrypted: ");
    dump_hex(decrypted, sizeof decrypted);

    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: Decrypted does not match input\n");
        return 1;
    }
    printf("OK: Round-trip successful\n");

    /* Test 2: Non-deterministic encryption (ND mode with 8-byte tweak) */
    memset(tweak_nd, 0, sizeof tweak_nd);
    tweak_nd[0] = 0xaa;
    tweak_nd[1] = 0xbb;
    tweak_nd[2] = 0xcc;
    tweak_nd[3] = 0xdd;
    tweak_nd[4] = 0xee;
    tweak_nd[5] = 0xff;
    tweak_nd[6] = 0x11;
    tweak_nd[7] = 0x22;

    printf("\nTest 2: Non-deterministic encryption (ND mode)\n");
    printf("Tweak: ");
    dump_hex(tweak_nd, sizeof tweak_nd);

    crypto_ipcrypt_nd_encrypt(nd_output, input, tweak_nd, key);
    printf("ND Encrypted: ");
    dump_hex(nd_output, sizeof nd_output);

    crypto_ipcrypt_nd_decrypt(decrypted, nd_output, key);
    printf("ND Decrypted: ");
    dump_hex(decrypted, sizeof decrypted);

    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: ND decrypted does not match input\n");
        return 1;
    }
    printf("OK: ND round-trip successful\n");

    /* Test 3: Non-deterministic encryption with extended tweak (NDX mode) */
    memset(ndx_key, 0x00, sizeof ndx_key);
    for (i = 0; i < sizeof ndx_key; i++) {
        ndx_key[i] = (unsigned char) (i + 1);
    }

    memset(tweak_ndx, 0, sizeof tweak_ndx);
    for (i = 0; i < sizeof tweak_ndx; i++) {
        tweak_ndx[i] = (unsigned char) (0xaa + i);
    }

    printf("\nTest 3: Non-deterministic encryption (NDX mode with 16-byte tweak)\n");
    printf("NDX Key: ");
    dump_hex(ndx_key, sizeof ndx_key);
    printf("NDX Tweak: ");
    dump_hex(tweak_ndx, sizeof tweak_ndx);

    crypto_ipcrypt_ndx_encrypt(ndx_output, input, tweak_ndx, ndx_key);
    printf("NDX Encrypted: ");
    dump_hex(ndx_output, sizeof ndx_output);

    crypto_ipcrypt_ndx_decrypt(decrypted, ndx_output, ndx_key);
    printf("NDX Decrypted: ");
    dump_hex(decrypted, sizeof decrypted);

    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: NDX decrypted does not match input\n");
        return 1;
    }
    printf("OK: NDX round-trip successful\n");

    /* Test 4: Keygen functions - skip random output in .exp */
    printf("\nTest 4: Key generation\n");
    crypto_ipcrypt_keygen(key);
    printf("Random key generated (skipped in output)\n");

    crypto_ipcrypt_ndx_keygen(ndx_key);
    printf("Random NDX key generated (skipped in output)\n");

    /* Test 5: Different inputs produce different outputs */
    printf("\nTest 5: Different inputs produce different outputs\n");
    memset(key, 0x42, sizeof key);

    for (i = 0; i < 4; i++) {
        memset(input, 0, sizeof input);
        input[10] = 0xff;
        input[11] = 0xff;
        input[15] = (unsigned char) i;

        crypto_ipcrypt_encrypt(output, input, key);
        printf("Input[%zu]: ", i);
        dump_hex(input, sizeof input);
        dump_hex(output, sizeof output);
    }

    /* Test 6: Verify deterministic encryption */
    printf("\nTest 6: Verify deterministic encryption\n");
    memset(key, 0x55, sizeof key);
    memset(input, 0xaa, sizeof input);

    crypto_ipcrypt_encrypt(output, input, key);
    crypto_ipcrypt_encrypt(decrypted, input, key);

    if (memcmp(output, decrypted, sizeof output) != 0) {
        printf("FAILED: Deterministic encryption produced different outputs\n");
        return 1;
    }
    printf("OK: Deterministic encryption verified\n");

    printf("\nTest 7: In-place encryption and decryption\n");

    crypto_ipcrypt_encrypt(output, input, key);
    memcpy(decrypted, input, sizeof input);
    crypto_ipcrypt_encrypt(decrypted, decrypted, key);
    if (memcmp(output, decrypted, sizeof output) != 0) {
        printf("FAILED: In-place encryption differs from out-of-place\n");
        return 1;
    }
    crypto_ipcrypt_decrypt(decrypted, decrypted, key);
    if (memcmp(input, decrypted, sizeof input) != 0) {
        printf("FAILED: In-place decryption does not match original\n");
        return 1;
    }
    printf("OK: In-place round-trip successful\n");

    printf("\nAll tests passed!\n");

    return 0;
}
