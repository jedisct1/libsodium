#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_ipcrypt.h"
#include "utils.h"

#include "private/common.h"

#if defined(HAVE_ARMCRYPTO) && defined(NATIVE_LITTLE_ENDIAN)

#    include "ipcrypt_armcrypto.h"

#    ifndef __ARM_FEATURE_CRYPTO
#        define __ARM_FEATURE_CRYPTO 1
#    endif
#    ifndef __ARM_FEATURE_AES
#        define __ARM_FEATURE_AES 1
#    endif

#    include <arm_neon.h>

#    ifdef __clang__
#        pragma clang attribute push(__attribute__((target("neon,crypto,aes"))), \
                                     apply_to = function)
#    elif defined(__GNUC__)
#        pragma GCC target("+simd+crypto")
#    endif

#    define ROUNDS 10

typedef uint64x2_t BlockVec;

#    define LOAD128(a)        vld1q_u64((const uint64_t *) (const void *) (a))
#    define STORE128(a, b)    vst1q_u64((uint64_t *) (void *) (a), (b))
#    define XOR128(a, b)      veorq_u64((a), (b))
#    define XOR128_3(a, b, c) veorq_u64(veorq_u64((a), (b)), (c))
#    define SET64x2(a, b)     vsetq_lane_u64((uint64_t) (a), vmovq_n_u64((uint64_t) (b)), 1)
#    define BYTESHL128(a, b) \
        vreinterpretq_u64_u8(vextq_s8(vdupq_n_s8(0), (int8x16_t) vreinterpretq_u8_u64(a), 16 - (b)))

#    define AES_XENCRYPT(block_vec, rkey) \
        vreinterpretq_u64_u8(             \
            vaesmcq_u8(vaeseq_u8(vreinterpretq_u8_u64(rkey), vreinterpretq_u8_u64(block_vec))))
#    define AES_XENCRYPTLAST(block_vec, rkey) \
        vreinterpretq_u64_u8(vaeseq_u8(vreinterpretq_u8_u64(rkey), vreinterpretq_u8_u64(block_vec)))
#    define AES_XDECRYPT(block_vec, rkey) \
        vreinterpretq_u64_u8(             \
            vaesimcq_u8(vaesdq_u8(vreinterpretq_u8_u64(rkey), vreinterpretq_u8_u64(block_vec))))
#    define AES_XDECRYPTLAST(block_vec, rkey) \
        vreinterpretq_u64_u8(vaesdq_u8(vreinterpretq_u8_u64(rkey), vreinterpretq_u8_u64(block_vec)))
#    define RKINVERT(rkey) vreinterpretq_u64_u8(vaesimcq_u8(vreinterpretq_u8_u64(rkey)))

#    define SHUFFLE32x4(x, a, b, c, d)                 \
        vreinterpretq_u64_u32(__builtin_shufflevector( \
            vreinterpretq_u32_u64(x), vreinterpretq_u32_u64(x), (a), (b), (c), (d)))

typedef BlockVec KeySchedule[1 + ROUNDS];

static BlockVec
AES_KEYGEN(BlockVec block_vec, const int rc)
{
    uint8x16_t       a = vaeseq_u8(vreinterpretq_u8_u64(block_vec), vmovq_n_u8(0));
    const uint8x16_t b =
        __builtin_shufflevector(a, a, 4, 1, 14, 11, 1, 14, 11, 4, 12, 9, 6, 3, 9, 6, 3, 12);
    const uint64x2_t c = SET64x2((uint64_t) rc << 32, (uint64_t) rc << 32);
    return XOR128(vreinterpretq_u64_u8(b), c);
}

static void
expand_key(KeySchedule rkeys, const uint8_t key[16])
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
aes_encrypt(uint8_t out[16], const uint8_t in[16], const KeySchedule rkeys)
{
    BlockVec t;
    size_t   i;

    t = AES_XENCRYPT(LOAD128(in), rkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        t = AES_XENCRYPT(t, rkeys[i]);
    }
    t = AES_XENCRYPTLAST(t, rkeys[i]);
    t = XOR128(t, rkeys[ROUNDS]);
    STORE128(out, t);
}

static void
aes_decrypt(uint8_t out[16], const uint8_t in[16], const KeySchedule rkeys)
{
    KeySchedule rkeys_inv;
    BlockVec    t;
    size_t      i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = RKINVERT(rkeys[ROUNDS - 1 - i]);
    }
    t = AES_XDECRYPT(LOAD128(in), rkeys[ROUNDS]);
    for (i = 0; i < ROUNDS - 2; i++) {
        t = AES_XDECRYPT(t, rkeys_inv[i]);
    }
    t = AES_XDECRYPTLAST(t, rkeys_inv[i]);
    t = XOR128(t, rkeys[0]);
    STORE128(out, t);
}

static BlockVec
tweak_expand(const uint8_t tweak[8])
{
    return vreinterpretq_u64_u32(vmovl_u16(vld1_u16((const uint16_t *) (tweak))));
}

static void
aes_encrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const KeySchedule rkeys)
{
    const BlockVec tweak_block = tweak_expand(tweak);
    BlockVec       t;
    size_t         i;

    t = AES_XENCRYPT(LOAD128(in), XOR128(tweak_block, rkeys[0]));
    for (i = 1; i < ROUNDS - 1; i++) {
        t = AES_XENCRYPT(t, XOR128(tweak_block, rkeys[i]));
    }
    t = AES_XENCRYPTLAST(t, XOR128(tweak_block, rkeys[i]));
    t = XOR128(t, XOR128(tweak_block, rkeys[ROUNDS]));
    STORE128(out, t);
}

static void
aes_decrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const KeySchedule rkeys)
{
    KeySchedule    rkeys_inv;
    const BlockVec tweak_block     = tweak_expand(tweak);
    const BlockVec tweak_block_inv = RKINVERT(tweak_block);
    BlockVec       t;
    size_t         i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = RKINVERT(rkeys[ROUNDS - 1 - i]);
    }
    t = AES_XDECRYPT(LOAD128(in), XOR128(tweak_block, rkeys[ROUNDS]));
    for (i = 0; i < ROUNDS - 2; i++) {
        t = AES_XDECRYPT(t, XOR128(tweak_block_inv, rkeys_inv[i]));
    }
    t = AES_XDECRYPTLAST(t, XOR128(tweak_block_inv, rkeys_inv[i]));
    t = XOR128(t, XOR128(tweak_block, rkeys[0]));
    STORE128(out, t);
}

static BlockVec
aes_xex_tweak(const uint8_t tweak[16], const KeySchedule tkeys)
{
    BlockVec tt;
    size_t   i;

    tt = AES_XENCRYPT(LOAD128(tweak), tkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        tt = AES_XENCRYPT(tt, tkeys[i]);
    }
    tt = AES_XENCRYPTLAST(tt, tkeys[i]);
    tt = XOR128(tt, tkeys[ROUNDS]);
    return tt;
}

static void
aes_xex_encrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const KeySchedule tkeys, const KeySchedule rkeys)
{
    const BlockVec tt = aes_xex_tweak(tweak, tkeys);
    BlockVec       t;
    size_t         i;

    t = AES_XENCRYPT(XOR128(LOAD128(in), tt), rkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        t = AES_XENCRYPT(t, rkeys[i]);
    }
    t = AES_XENCRYPTLAST(t, rkeys[i]);
    t = XOR128_3(t, rkeys[ROUNDS], tt);
    STORE128(out, t);
}

static void
aes_xex_decrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const KeySchedule tkeys, const KeySchedule rkeys)
{
    KeySchedule    rkeys_inv;
    const BlockVec tt = aes_xex_tweak(tweak, tkeys);
    BlockVec       t;
    size_t         i;

    for (i = 0; i < ROUNDS - 1; i++) {
        rkeys_inv[i] = RKINVERT(rkeys[ROUNDS - 1 - i]);
    }
    t = AES_XDECRYPT(XOR128(LOAD128(in), tt), rkeys[ROUNDS]);
    for (i = 0; i < ROUNDS - 2; i++) {
        t = AES_XDECRYPT(t, rkeys_inv[i]);
    }
    t = AES_XDECRYPTLAST(t, rkeys_inv[i]);
    t = XOR128_3(t, rkeys[0], tt);
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

struct ipcrypt_implementation ipcrypt_armcrypto_implementation = {
    SODIUM_C99(.encrypt =) encrypt,         SODIUM_C99(.decrypt =) decrypt,
    SODIUM_C99(.nd_encrypt =) nd_encrypt,   SODIUM_C99(.nd_decrypt =) nd_decrypt,
    SODIUM_C99(.ndx_encrypt =) ndx_encrypt, SODIUM_C99(.ndx_decrypt =) ndx_decrypt
};

#    ifdef __clang__
#        pragma clang attribute pop
#    endif

#endif
