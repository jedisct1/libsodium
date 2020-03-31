
#define TEST_NAME "blabla2000"
#include "cmptest.h"

static const unsigned char key[32] = {
    0x1b, 0x27, 0x55, 0x64, 0x73, 0xe9, 0x85,
    0xd4, 0x62, 0xcd, 0x51, 0x19, 0x7a, 0x9a,
    0x46, 0xc7, 0x60, 0x09, 0x54, 0x9e, 0xac,
    0x64, 0x74, 0xf2, 0x06, 0xc4, 0xee, 0x08,
    0x44, 0xf6, 0x83, 0x89
};

static const unsigned char nonce[16] = {
    0x69, 0x69, 0x6e, 0xe9, 0x55, 0xb6,
    0x2b, 0x73, 0xcd, 0x62, 0xbd, 0xa8,
    0x75, 0xfc, 0x73, 0xd6
};

int
main(void)
{
    unsigned char  h[32];
    char          *hex;
    unsigned char *output;
    size_t         sizeof_hex = 17 * 64 * 2 + 1;
    size_t         sizeof_output = 4194304;
    int            i;

    output = (unsigned char *) sodium_malloc(sizeof_output);
    hex = (char *) sodium_malloc(sizeof_hex);

    crypto_stream_blabla2000(output, sizeof_output, nonce, key);
    crypto_generichash(h, sizeof h, output, sizeof_output, NULL, 0U);
    sodium_bin2hex(hex, sizeof_hex, h, sizeof h);
    printf("%s\n", hex);

    assert(crypto_stream_blabla2000_keybytes() > 0U);
    assert(crypto_stream_blabla2000_keybytes() == crypto_stream_blabla2000_KEYBYTES);
    assert(crypto_stream_blabla2000_noncebytes() > 0U);
    assert(crypto_stream_blabla2000_noncebytes() == crypto_stream_blabla2000_NONCEBYTES);
    assert(crypto_stream_blabla2000_messagebytes_max() > 0U);
    assert(crypto_stream_blabla2000_messagebytes_max() == crypto_stream_blabla2000_MESSAGEBYTES_MAX);

    return 0;
}
