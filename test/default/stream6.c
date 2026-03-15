#define TEST_NAME "stream6"
#include "cmptest.h"

int
main(void)
{
    unsigned char  key[crypto_stream_salsa2012_KEYBYTES];
    unsigned char  nonce[crypto_stream_salsa2012_NONCEBYTES];
    unsigned char *buf;
    unsigned char *out;

    memset(key, 0x01, sizeof key);
    memset(nonce, 0x02, sizeof nonce);

    buf = (unsigned char *) sodium_malloc(256);
    out = (unsigned char *) sodium_malloc(256);

    assert(crypto_stream_salsa2012_xor(out, buf, 0, nonce, key) == 0);
    printf("xor_zero_len: ok\n");

    memset(buf, 0, 100);
    crypto_stream_salsa2012_xor(out, buf, 100, nonce, key);
    {
        unsigned char stream_out[100];

        crypto_stream_salsa2012(stream_out, 100, nonce, key);
        assert(memcmp(out, stream_out, 100) == 0);
    }
    printf("xor_tail_block: ok\n");

    memset(buf, 0x55, 137);
    crypto_stream_salsa2012_xor(out, buf, 137, nonce, key);
    crypto_stream_salsa2012_xor(out, out, 137, nonce, key);
    assert(memcmp(out, buf, 137) == 0);
    printf("xor_roundtrip_137: ok\n");

    memset(out, 0xab, 1);
    crypto_stream_salsa2012(out, 0, nonce, key);
    assert(out[0] == 0xab);
    printf("stream_zero_len: ok\n");

    memset(buf, 0x42, 256);
    crypto_stream_salsa2012_xor(out, buf, 256, nonce, key);
    crypto_stream_salsa2012_xor(out, out, 256, nonce, key);
    assert(memcmp(out, buf, 256) == 0);
    printf("xor_roundtrip_256: ok\n");

    sodium_free(buf);
    sodium_free(out);

    return 0;
}
