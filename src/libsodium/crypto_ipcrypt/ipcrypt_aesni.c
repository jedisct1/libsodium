#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_ipcrypt.h"
#include "utils.h"

#include "private/common.h"

#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)

#    include "ipcrypt_aesni.h"

#    ifdef __clang__
#        pragma clang attribute push(__attribute__((target("aes,avx"))), apply_to = function)
#    elif defined(__GNUC__)
#        pragma GCC target("aes,avx")
#    endif

#    include <immintrin.h>
#    include <wmmintrin.h>

#    define ROUNDS 10

typedef __m128i BlockVec;

#    define LOAD128(a)                       _mm_loadu_si128((const BlockVec *) (const void *) (a))
#    define STORE128(a, b)                   _mm_storeu_si128((BlockVec *) (void *) (a), (b))
#    define AES_ENCRYPT(block_vec, rkey)     _mm_aesenc_si128((block_vec), (rkey))
#    define AES_ENCRYPTLAST(block_vec, rkey) _mm_aesenclast_si128((block_vec), (rkey))
#    define AES_DECRYPT(block_vec, rkey)     _mm_aesdec_si128((block_vec), (rkey))
#    define AES_DECRYPTLAST(block_vec, rkey) _mm_aesdeclast_si128((block_vec), (rkey))
#    define AES_KEYGEN(block_vec, rc)        _mm_aeskeygenassist_si128((block_vec), (rc))
#    define AES_IMC(rkey)                    _mm_aesimc_si128(rkey)
#    define XOR128(a, b)                     _mm_xor_si128((a), (b))
#    define XOR128_3(a, b, c)                _mm_xor_si128(_mm_xor_si128((a), (b)), (c))
#    define SET64x2(a, b)                    _mm_set_epi64x((uint64_t) (a), (uint64_t) (b))
#    define BYTESHL128(a, b)                 _mm_slli_si128(a, b)
#    define SHUFFLE32x4(x, a, b, c, d)       _mm_shuffle_epi32((x), _MM_SHUFFLE((d), (c), (b), (a)))

typedef BlockVec KeySchedule[1 + ROUNDS];

static void
expand_key(BlockVec *rkeys, const uint8_t key[16])
{
    BlockVec t, s;
    size_t   i = 0;

#    define EXPAND_KEY(RC)                        \
        rkeys[i++] = t;                           \
        s          = AES_KEYGEN(t, RC);           \
        t          = XOR128(t, BYTESHL128(t, 4)); \
        t          = XOR128(t, BYTESHL128(t, 8)); \
        t          = XOR128(t, SHUFFLE32x4(s, 3, 3, 3, 3));

    t = LOAD128(key);
    EXPAND_KEY(0x01);
    EXPAND_KEY(0x02);
    EXPAND_KEY(0x04);
    EXPAND_KEY(0x08);
    EXPAND_KEY(0x10);
    EXPAND_KEY(0x20);
    EXPAND_KEY(0x40);
    EXPAND_KEY(0x80);
    EXPAND_KEY(0x1b);
    EXPAND_KEY(0x36);
    rkeys[i++] = t;
}

static void
aes_encrypt(uint8_t out[16], const uint8_t in[16], const BlockVec *rkeys)
{
    BlockVec t;
    size_t   i;

    t = XOR128(LOAD128(in), rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENCRYPT(t, rkeys[i]);
    }
    t = AES_ENCRYPTLAST(t, rkeys[ROUNDS]);
    STORE128(out, t);
}

static void
aes_decrypt(uint8_t out[16], const uint8_t in[16], const BlockVec *rkeys)
{
    KeySchedule rkeys_inv;
    BlockVec    t;
    size_t      i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = AES_IMC(rkeys[ROUNDS - 1 - i]);
    }
    t = XOR128(LOAD128(in), rkeys[ROUNDS]);
    for (i = 0; i < ROUNDS - 1; i++) {
        t = AES_DECRYPT(t, rkeys_inv[i]);
    }
    t = AES_DECRYPTLAST(t, rkeys[0]);
    STORE128(out, t);
}

static BlockVec
tweak_expand(const uint8_t tweak[8])
{
    return _mm_shuffle_epi8(_mm_loadl_epi64((const __m128i *) (const void *) tweak),
                            _mm_setr_epi8(0x00, 0x01, -128, -128, 0x02, 0x03, -128, -128, 0x04,
                                          0x05, -128, -128, 0x06, 0x07, -128, -128));
}

static void
aes_encrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const BlockVec *rkeys)
{
    const BlockVec tweak_block = tweak_expand(tweak);
    BlockVec       t;
    size_t         i;

    t = XOR128_3(LOAD128(in), tweak_block, rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENCRYPT(t, XOR128(tweak_block, rkeys[i]));
    }
    t = AES_ENCRYPTLAST(t, XOR128(tweak_block, rkeys[ROUNDS]));
    STORE128(out, t);
}

static void
aes_decrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const BlockVec *rkeys)
{
    KeySchedule    rkeys_inv;
    const BlockVec tweak_block     = tweak_expand(tweak);
    const BlockVec tweak_block_inv = AES_IMC(tweak_block);
    BlockVec       t;
    size_t         i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = AES_IMC(rkeys[ROUNDS - 1 - i]);
    }
    t = XOR128_3(LOAD128(in), tweak_block, rkeys[ROUNDS]);
    for (i = 0; i < ROUNDS - 1; i++) {
        t = AES_DECRYPT(t, XOR128(tweak_block_inv, rkeys_inv[i]));
    }
    t = AES_DECRYPTLAST(t, XOR128(tweak_block, rkeys[0]));
    STORE128(out, t);
}

static BlockVec
aes_xex_tweak(const uint8_t tweak[16], const BlockVec *tkeys)
{
    BlockVec tt;
    size_t   i;

    tt = XOR128(LOAD128(tweak), tkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        tt = AES_ENCRYPT(tt, tkeys[i]);
    }
    tt = AES_ENCRYPTLAST(tt, tkeys[ROUNDS]);
    return tt;
}

static void
aes_xex_encrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const BlockVec *tkeys, const BlockVec *rkeys)
{
    const BlockVec tt = aes_xex_tweak(tweak, tkeys);
    BlockVec       t;
    size_t         i;

    t = XOR128(XOR128(LOAD128(in), tt), rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENCRYPT(t, rkeys[i]);
    }
    t = AES_ENCRYPTLAST(t, XOR128(rkeys[ROUNDS], tt));
    STORE128(out, t);
}

static void
aes_xex_decrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const BlockVec *tkeys, const BlockVec *rkeys)
{
    KeySchedule    rkeys_inv;
    const BlockVec tt = aes_xex_tweak(tweak, tkeys);
    BlockVec       t;
    size_t         i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = AES_IMC(rkeys[ROUNDS - 1 - i]);
    }
    t = XOR128(XOR128(LOAD128(in), tt), rkeys[ROUNDS]);
    for (i = 0; i < ROUNDS - 1; i++) {
        t = AES_DECRYPT(t, rkeys_inv[i]);
    }
    t = AES_DECRYPTLAST(t, XOR128(rkeys[0], tt));
    STORE128(out, t);
}

static void
encrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule rkeys;

    expand_key(rkeys, k);
    aes_encrypt(out, in, rkeys);
}

static void
decrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule rkeys;

    expand_key(rkeys, k);
    aes_decrypt(out, in, rkeys);
}

static void
nd_encrypt(uint8_t *out, const uint8_t *in, const uint8_t *t, const uint8_t *k)
{
    KeySchedule rkeys;

    expand_key(rkeys, k);
    memcpy(out, t, 8);
    aes_encrypt_with_tweak(out + 8, in, t, rkeys);
}

static void
nd_decrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule rkeys;

    expand_key(rkeys, k);
    aes_decrypt_with_tweak(out, in + 8, in, rkeys);
}

static void
ndx_encrypt(uint8_t *out, const uint8_t *in, const uint8_t *t, const uint8_t *k)
{
    KeySchedule tkeys;
    KeySchedule rkeys;
    uint8_t     diff[16];
    size_t      i;
    uint8_t     d;

    expand_key(tkeys, k + 16);
    expand_key(rkeys, k);

    STORE128(diff, XOR128(tkeys[ROUNDS / 2], rkeys[ROUNDS / 2]));
    d = 0;
    for (i = 0; i < 16; i++) {
        d |= diff[i];
    }
    if (d == 0) {
        for (i = 0; i < 16; i++) {
            diff[i] = k[i] ^ 0x5a;
        }
        expand_key(rkeys, diff);
    }

    memcpy(out, t, 16);
    aes_xex_encrypt(out + 16, in, t, tkeys, rkeys);
}

static void
ndx_decrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule tkeys;
    KeySchedule rkeys;
    uint8_t     diff[16];
    size_t      i;
    uint8_t     d;

    expand_key(tkeys, k + 16);
    expand_key(rkeys, k);

    STORE128(diff, XOR128(tkeys[ROUNDS / 2], rkeys[ROUNDS / 2]));
    d = 0;
    for (i = 0; i < 16; i++) {
        d |= diff[i];
    }
    if (d == 0) {
        for (i = 0; i < 16; i++) {
            diff[i] = k[i] ^ 0x5a;
        }
        expand_key(rkeys, diff);
    }

    aes_xex_decrypt(out, in + 16, in, tkeys, rkeys);
}

struct ipcrypt_implementation ipcrypt_aesni_implementation = {
    SODIUM_C99(.encrypt =) encrypt,         SODIUM_C99(.decrypt =) decrypt,
    SODIUM_C99(.nd_encrypt =) nd_encrypt,   SODIUM_C99(.nd_decrypt =) nd_decrypt,
    SODIUM_C99(.ndx_encrypt =) ndx_encrypt, SODIUM_C99(.ndx_decrypt =) ndx_decrypt
};

#    ifdef __clang__
#        pragma clang attribute pop
#    endif

#endif
