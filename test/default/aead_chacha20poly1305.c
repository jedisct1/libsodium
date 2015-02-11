
#define TEST_NAME "aead_chacha20poly1305"
#include "cmptest.h"

static int
tv(void)
{
    static unsigned char firstkey[crypto_aead_chacha20poly1305_KEYBYTES]
        = { 0x42, 0x90, 0xbc, 0xb1, 0x54, 0x17, 0x35, 0x31, 0xf3, 0x14, 0xaf,
            0x57, 0xf3, 0xbe, 0x3b, 0x50, 0x06, 0xda, 0x37, 0x1e, 0xce, 0x27,
            0x2a, 0xfa, 0x1b, 0x5d, 0xbd, 0xd1, 0x10, 0x0a, 0x10, 0x07 };
    static unsigned char m[10U]
        = { 0x86, 0xd0, 0x99, 0x74, 0x84, 0x0b, 0xde, 0xd2, 0xa5, 0xca };
    static unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES]
        = { 0xcd, 0x7c, 0xf6, 0x7b, 0xe3, 0x9c, 0x79, 0x4a };
    static unsigned char ad[10U]
        = { 0x87, 0xe2, 0x29, 0xd4, 0x50, 0x08, 0x45, 0xa0, 0x79, 0xc0 };
    static unsigned char c[10U + crypto_aead_chacha20poly1305_ABYTES];

    unsigned char m2[10U];
    unsigned long long clen;
    unsigned long long m2len;
    size_t i;

    crypto_aead_chacha20poly1305_encrypt(c, &clen, m, sizeof m, ad, sizeof ad,
                                         NULL, nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");

    if (crypto_aead_chacha20poly1305_decrypt(m2, &m2len, NULL, c, sizeof c, ad,
                                             sizeof ad, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_decrypt() failed\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set\n");
    }
    if (memcmp(m, m2, sizeof m) != 0) {
        printf("m != m2\n");
    }

    for (i = 0U; i < sizeof c; i++) {
        c[i] ^= (i + 1U);
        if (crypto_aead_chacha20poly1305_decrypt(m2, NULL, NULL, c, sizeof c,
                                                 ad, sizeof ad, nonce, firstkey)
            == 0 || memcmp(m, m2, sizeof m) == 0) {
            printf("message can be forged\n");
        }
        c[i] ^= (i + 1U);
    }

    crypto_aead_chacha20poly1305_encrypt(c, &clen, m, sizeof m, NULL, 0U, NULL,
                                         nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set (adlen=0)\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");

    if (crypto_aead_chacha20poly1305_decrypt(m2, &m2len, NULL, c, sizeof c,
                                             NULL, 0U, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_decrypt() failed (adlen=0)\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set (adlen=0)\n");
    }
    if (memcmp(m, m2, sizeof m) != 0) {
        printf("m != m2 (adlen=0)\n");
    }

    if (crypto_aead_chacha20poly1305_decrypt(
            m2, &m2len, NULL, c, crypto_aead_chacha20poly1305_ABYTES / 2, NULL,
            0U, nonce, firstkey) != -1) {
        printf("crypto_aead_chacha20poly1305_decrypt() worked with a short "
               "ciphertext\n");
    }
    if (crypto_aead_chacha20poly1305_decrypt(m2, &m2len, NULL, c, 0U, NULL, 0U,
                                             nonce, firstkey) != -1) {
        printf("crypto_aead_chacha20poly1305_decrypt() worked with an empty "
               "ciphertext\n");
    }

    memcpy(c, m, sizeof m);
    crypto_aead_chacha20poly1305_encrypt(c, &clen, c, sizeof m, NULL, 0U, NULL,
                                         nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set (adlen=0)\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");

    if (crypto_aead_chacha20poly1305_decrypt(c, &m2len, NULL, c, sizeof c,
                                             NULL, 0U, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_decrypt() failed (adlen=0)\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set (adlen=0)\n");
    }
    if (memcmp(m, c, sizeof m) != 0) {
        printf("m != c (adlen=0)\n");
    }

    assert(crypto_aead_chacha20poly1305_keybytes() > 0U);
    assert(crypto_aead_chacha20poly1305_npubbytes() > 0U);
    assert(crypto_aead_chacha20poly1305_nsecbytes() == 0U);

    return 0;
}

static int
tv_ietf(void)
{
    static unsigned char firstkey[crypto_aead_chacha20poly1305_KEYBYTES]
        = {
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
            0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
            0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
        };
#define MESSAGE "Ladies and Gentlemen of the class of '99: If I could offer you " \
"only one tip for the future, sunscreen would be it."
    static unsigned char m[114U];
    static unsigned char nonce[crypto_aead_chacha20poly1305_IETF_NPUBBYTES]
        = { 0x07, 0x00, 0x00, 0x00,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };
    static unsigned char ad[12U]
        = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };
    static unsigned char c[114U + crypto_aead_chacha20poly1305_ABYTES];

    unsigned char m2[114U];
    unsigned long long clen;
    unsigned long long m2len;
    size_t i;

    assert(sizeof MESSAGE - 1U == sizeof m);
    memcpy(m, MESSAGE, sizeof m);
    crypto_aead_chacha20poly1305_ietf_encrypt(c, &clen, m, sizeof m, ad, sizeof ad,
                                              NULL, nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");

    if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, sizeof c, ad,
                                                  sizeof ad, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_ietf_decrypt() failed\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set\n");
    }
    if (memcmp(m, m2, sizeof m) != 0) {
        printf("m != m2\n");
    }

    for (i = 0U; i < sizeof c; i++) {
        c[i] ^= (i + 1U);
        if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, NULL, NULL, c, sizeof c,
                                                      ad, sizeof ad, nonce, firstkey)
            == 0 || memcmp(m, m2, sizeof m) == 0) {
            printf("message can be forged\n");
        }
        c[i] ^= (i + 1U);
    }
    crypto_aead_chacha20poly1305_ietf_encrypt(c, &clen, m, sizeof m, NULL, 0U, NULL,
                                              nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set (adlen=0)\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");
    if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, sizeof c,
                                                  NULL, 0U, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_ietf_decrypt() failed (adlen=0)\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set (adlen=0)\n");
    }
    if (memcmp(m, m2, sizeof m) != 0) {
        printf("m != m2 (adlen=0)\n");
    }

    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            m2, &m2len, NULL, c, crypto_aead_chacha20poly1305_ABYTES / 2, NULL,
            0U, nonce, firstkey) != -1) {
        printf("crypto_aead_chacha20poly1305_ietf_decrypt() worked with a short "
               "ciphertext\n");
    }
    if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, 0U, NULL, 0U,
                                                  nonce, firstkey) != -1) {
        printf("crypto_aead_chacha20poly1305_ietf_decrypt() worked with an empty "
               "ciphertext\n");
    }

    memcpy(c, m, sizeof m);
    crypto_aead_chacha20poly1305_ietf_encrypt(c, &clen, c, sizeof m, NULL, 0U, NULL,
                                              nonce, firstkey);
    if (clen != sizeof m + crypto_aead_chacha20poly1305_abytes()) {
        printf("clen is not properly set (adlen=0)\n");
    }
    for (i = 0U; i < sizeof c; ++i) {
        printf(",0x%02x", (unsigned int)c[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");

    if (crypto_aead_chacha20poly1305_ietf_decrypt(c, &m2len, NULL, c, sizeof c,
                                             NULL, 0U, nonce, firstkey) != 0) {
        printf("crypto_aead_chacha20poly1305_ietf_decrypt() failed (adlen=0)\n");
    }
    if (m2len != sizeof c - crypto_aead_chacha20poly1305_abytes()) {
        printf("m2len is not properly set (adlen=0)\n");
    }
    if (memcmp(m, c, sizeof m) != 0) {
        printf("m != c (adlen=0)\n");
    }

    assert(crypto_aead_chacha20poly1305_keybytes() > 0U);
    assert(crypto_aead_chacha20poly1305_ietf_npubbytes() > 0U);
    assert(crypto_aead_chacha20poly1305_nsecbytes() == 0U);

    return 0;
}

int
main(void)
{
    tv();
    tv_ietf();

    return 0;
}
