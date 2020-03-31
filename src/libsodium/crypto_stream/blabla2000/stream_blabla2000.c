#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_stream_blabla2000.h"
#include "private/common.h"
#include "randombytes.h"

#define crypto_stream_blabla2000_ROUNDS 2000

#define crypto_stream_blabla2000_BLOCKBYTES 128

#define CRYPTO_STREAM_BLABLA2000_QUARTERROUND(a, b, c, d) \
    a += b;                                               \
    d = ROTR64(d ^ a, 32);                                \
    c += d;                                               \
    b = ROTR64(b ^ c, 24);                                \
    a += b;                                               \
    d = ROTR64(d ^ a, 16);                                \
    c += d;                                               \
    b = ROTR64(b ^ c, 63)

static void
crypto_stream_blabla2000_rounds(uint64_t st[16])
{
    int i;

    for (i = 0; i < crypto_stream_blabla2000_ROUNDS; i += 2) {
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[0], st[4], st[8], st[12]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[1], st[5], st[9], st[13]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[2], st[6], st[10], st[14]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[3], st[7], st[11], st[15]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[0], st[5], st[10], st[15]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[1], st[6], st[11], st[12]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[2], st[7], st[8], st[13]);
        CRYPTO_STREAM_BLABLA2000_QUARTERROUND(st[3], st[4], st[9], st[14]);
    }
}

static void
crypto_stream_blabla2000_update(uint64_t ks[16], uint64_t st[16])
{
    int i;

    memcpy(ks, st, 8 * 16);
    crypto_stream_blabla2000_rounds(st);
    for (i = 0; i < 16; i++) {
        ks[i] += st[i];
    }
    ++st[13];
}

static void
crypto_stream_blabla2000_init(uint64_t st[16],
                              const unsigned char nonce[crypto_stream_blabla2000_NONCEBYTES],
                              const unsigned char key[crypto_stream_blabla2000_KEYBYTES])
{
    int i;

    st[0] = 0x6170786593810fab;
    st[1] = 0x3320646ec7398aee;
    st[2] = 0x79622d3217318274;
    st[3] = 0x6b206574babadada;
    for (i = 0; i < 4; i++) {
        st[4 + i] = LOAD64_LE(&key[8 * i]);
    }
    st[8]  = 0x2ae36e593e46ad5f;
    st[9]  = 0xb68f143029225fc9;
    st[10] = 0x8da1e08468303aa6;
    st[11] = 0xa48a209acd50a4a7;
    st[12] = 0x7fdc12f23f90778c;
    st[13] = 1;
    st[14] = LOAD64_LE(&nonce[8 * 0]);
    st[15] = LOAD64_LE(&nonce[8 * 1]);
}

int
crypto_stream_blabla2000_xor(unsigned char *c, const unsigned char *m,
                             unsigned long long len,
                             const unsigned char nonce[crypto_stream_blabla2000_NONCEBYTES],
                             const unsigned char key[crypto_stream_blabla2000_KEYBYTES])
{
    unsigned char tmp[crypto_stream_blabla2000_BLOCKBYTES];
    uint64_t      ks[16];
    uint64_t      st[16];
    uint64_t      x;
    int           i;

    crypto_stream_blabla2000_init(st, nonce, key);
    while (len >= crypto_stream_blabla2000_BLOCKBYTES) {
        crypto_stream_blabla2000_update(ks, st);
        for (i = 0; i < 16; i++) {
            x = ks[i] ^ LOAD64_LE(m + 8 * i);
            STORE64_LE(c + 8 * i, x);
        }
        c += crypto_stream_blabla2000_BLOCKBYTES;
        m += crypto_stream_blabla2000_BLOCKBYTES;
        len -= crypto_stream_blabla2000_BLOCKBYTES;
    }
    if (len > 0) {
        crypto_stream_blabla2000_update(ks, st);
        memset(tmp, 0, crypto_stream_blabla2000_BLOCKBYTES);
        for (i = 0; i < (int) len; i++) {
            tmp[i] = m[i];
        }
        for (i = 0; i < 16; i++) {
            x = ks[i] ^ LOAD64_LE(tmp + 8 * i);
            STORE64_LE(tmp + 8 * i, x);
        }
        for (i = 0; i < (int) len; i++) {
            c[i] = tmp[i];
        }
    }
    return 0;
}

int
crypto_stream_blabla2000(unsigned char *c, unsigned long long len,
                         const unsigned char nonce[crypto_stream_blabla2000_NONCEBYTES],
                         const unsigned char key[crypto_stream_blabla2000_KEYBYTES])
{
    memset(c, 0, len);
    return crypto_stream_blabla2000_xor(c, c, len, nonce, key);
}

void
crypto_stream_blabla2000_keygen(unsigned char k[crypto_stream_blabla2000_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_blabla2000_KEYBYTES);
}

size_t
crypto_stream_blabla2000_keybytes(void)
{
    return crypto_stream_blabla2000_KEYBYTES;
}

size_t
crypto_stream_blabla2000_noncebytes(void)
{
    return crypto_stream_blabla2000_NONCEBYTES;
}

size_t
crypto_stream_blabla2000_messagebytes_max(void)
{
    return crypto_stream_blabla2000_MESSAGEBYTES_MAX;
}
