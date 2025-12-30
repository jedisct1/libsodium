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
        vreinterpretq_u64_u8(vextq_s8(vdupq_n_s8(0), vreinterpretq_s8_u64(a), 16 - (b)))

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

#    define SHUFFLE32x4_3333(x) vreinterpretq_u64_u32(vdupq_laneq_u32(vreinterpretq_u32_u64(x), 3))

typedef BlockVec KeySchedule[1 + ROUNDS];

static BlockVec
AES_KEYGEN(BlockVec block_vec, const int rc)
{
    static const uint8_t aes_keygen_shuffle[16] = {
        4, 1, 14, 11, 1, 14, 11, 4, 12, 9, 6, 3, 9, 6, 3, 12,
    };
    uint8x16_t       a = vaeseq_u8(vreinterpretq_u8_u64(block_vec), vmovq_n_u8(0));
    const BlockVec   b = vreinterpretq_u64_u8(vqtbl1q_u8(a, vld1q_u8(aes_keygen_shuffle)));
    const uint64x2_t c = SET64x2((uint64_t) rc << 32, (uint64_t) rc << 32);
    return XOR128(b, c);
}

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
        t          = XOR128(t, SHUFFLE32x4_3333(s));

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

    t = AES_XENCRYPT(LOAD128(in), rkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        t = AES_XENCRYPT(t, rkeys[i]);
    }
    t = AES_XENCRYPTLAST(t, rkeys[i]);
    t = XOR128(t, rkeys[ROUNDS]);
    STORE128(out, t);
}

static void
aes_decrypt(uint8_t out[16], const uint8_t in[16], const BlockVec *rkeys)
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
                       const BlockVec *rkeys)
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
                       const BlockVec *rkeys)
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
aes_xex_tweak(const uint8_t tweak[16], const BlockVec *tkeys)
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
                const BlockVec *tkeys, const BlockVec *rkeys)
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
                const BlockVec *tkeys, const BlockVec *rkeys)
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

static int
is_ipv4_mapped(const uint8_t ip16[16])
{
    static const uint8_t ipv4_mapped_prefix[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };

    return memcmp(ip16, ipv4_mapped_prefix, 12) == 0;
}

static uint8_t
pfx_get_bit(const uint8_t ip16[16], unsigned int bit_index)
{
    return (ip16[15 - bit_index / 8] >> (bit_index % 8)) & 1;
}

static void
pfx_set_bit(uint8_t ip16[16], const unsigned int bit_index, const uint8_t bit_value)
{
    const size_t  byte_index = 15 - bit_index / 8;
    const uint8_t bit_mask   = (uint8_t) (1 << (bit_index % 8));
    uint8_t       mask       = (uint8_t) -((bit_value & 1));

#    if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("" : "+r"(mask)::);
#    endif
    ip16[byte_index] = (ip16[byte_index] & ~bit_mask) | (bit_mask & mask);
}

static void
pfx_shift_left(uint8_t ip16[16])
{
    BlockVec       v       = LOAD128(ip16);
    const BlockVec shl     = vshlq_n_u8(vreinterpretq_u8_u64(v), 1);
    const BlockVec msb     = vshrq_n_u8(vreinterpretq_u8_u64(v), 7);
    const BlockVec zero    = vdupq_n_u8(0);
    const BlockVec carries = vextq_u8(vreinterpretq_u8_u64(msb), zero, 1);
    v                      = vreinterpretq_u64_u8(vorrq_u8(shl, carries));
    STORE128(ip16, v);
}

static void
pfx_pad_prefix(uint8_t padded_prefix[16], unsigned int prefix_len_bits)
{
    memset(padded_prefix, 0, 16);
    if (prefix_len_bits == 0) {
        padded_prefix[15] = 0x01;
    } else {
        padded_prefix[3]  = 0x01;
        padded_prefix[14] = 0xff;
        padded_prefix[15] = 0xff;
    }
}

static void
pfx_encrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule  k1keys;
    KeySchedule  k2keys;
    uint8_t      diff[16];
    uint8_t      encrypted[16];
    uint8_t      padded_prefix[16];
    uint8_t      t[16];
    BlockVec     e1, e2, e;
    unsigned int prefix_start = 0;
    unsigned int prefix_len_bits;
    unsigned int bit_pos;
    uint8_t      cipher_bit;
    uint8_t      original_bit;
    size_t       i;
    uint8_t      d;

    expand_key(k1keys, k);
    expand_key(k2keys, k + 16);

    STORE128(diff, XOR128(k1keys[ROUNDS / 2], k2keys[ROUNDS / 2]));
    d = 0;
    for (i = 0; i < 16; i++) {
        d |= diff[i];
    }
    if (d == 0) {
        for (i = 0; i < 16; i++) {
            diff[i] = k[i] ^ 0x5a;
        }
        expand_key(k2keys, diff);
    }

    if (is_ipv4_mapped(in)) {
        prefix_start = 96;
    }

    pfx_pad_prefix(padded_prefix, prefix_start);

    memset(encrypted, 0, 16);
    if (prefix_start == 96) {
        encrypted[10] = 0xff;
        encrypted[11] = 0xff;
    }

    for (prefix_len_bits = prefix_start; prefix_len_bits < 128; prefix_len_bits++) {
        e1 = AES_XENCRYPT(LOAD128(padded_prefix), k1keys[0]);
        e2 = AES_XENCRYPT(LOAD128(padded_prefix), k2keys[0]);
        for (i = 1; i < ROUNDS - 1; i++) {
            e1 = AES_XENCRYPT(e1, k1keys[i]);
            e2 = AES_XENCRYPT(e2, k2keys[i]);
        }
        e1 = AES_XENCRYPTLAST(e1, k1keys[i]);
        e2 = AES_XENCRYPTLAST(e2, k2keys[i]);
        e1 = XOR128(e1, k1keys[ROUNDS]);
        e2 = XOR128(e2, k2keys[ROUNDS]);

        e = XOR128(e1, e2);
        STORE128(t, e);

        cipher_bit   = t[15] & 1;
        bit_pos      = 127 - prefix_len_bits;
        original_bit = pfx_get_bit(in, bit_pos);
        pfx_set_bit(encrypted, bit_pos, original_bit ^ cipher_bit);

        pfx_shift_left(padded_prefix);
        pfx_set_bit(padded_prefix, 0, original_bit);
    }

    memcpy(out, encrypted, 16);
}

static void
pfx_decrypt(uint8_t *out, const uint8_t *in, const uint8_t *k)
{
    KeySchedule  k1keys;
    KeySchedule  k2keys;
    uint8_t      diff[16];
    uint8_t      decrypted[16];
    uint8_t      padded_prefix[16];
    uint8_t      t[16];
    BlockVec     e1, e2, e;
    unsigned int prefix_start = 0;
    unsigned int prefix_len_bits;
    unsigned int bit_pos;
    uint8_t      cipher_bit;
    uint8_t      encrypted_bit;
    uint8_t      original_bit;
    size_t       i;
    uint8_t      d;

    expand_key(k1keys, k);
    expand_key(k2keys, k + 16);

    STORE128(diff, XOR128(k1keys[ROUNDS / 2], k2keys[ROUNDS / 2]));
    d = 0;
    for (i = 0; i < 16; i++) {
        d |= diff[i];
    }
    if (d == 0) {
        for (i = 0; i < 16; i++) {
            diff[i] = k[i] ^ 0x5a;
        }
        expand_key(k2keys, diff);
    }

    if (is_ipv4_mapped(in)) {
        prefix_start = 96;
    }

    pfx_pad_prefix(padded_prefix, prefix_start);

    memset(decrypted, 0, 16);
    if (prefix_start == 96) {
        decrypted[10] = 0xff;
        decrypted[11] = 0xff;
    }

    for (prefix_len_bits = prefix_start; prefix_len_bits < 128; prefix_len_bits++) {
        e1 = AES_XENCRYPT(LOAD128(padded_prefix), k1keys[0]);
        e2 = AES_XENCRYPT(LOAD128(padded_prefix), k2keys[0]);
        for (i = 1; i < ROUNDS - 1; i++) {
            e1 = AES_XENCRYPT(e1, k1keys[i]);
            e2 = AES_XENCRYPT(e2, k2keys[i]);
        }
        e1 = AES_XENCRYPTLAST(e1, k1keys[i]);
        e2 = AES_XENCRYPTLAST(e2, k2keys[i]);
        e1 = XOR128(e1, k1keys[ROUNDS]);
        e2 = XOR128(e2, k2keys[ROUNDS]);

        e = XOR128(e1, e2);
        STORE128(t, e);

        cipher_bit    = t[15] & 1;
        bit_pos       = 127 - prefix_len_bits;
        encrypted_bit = pfx_get_bit(in, bit_pos);
        original_bit  = encrypted_bit ^ cipher_bit;
        pfx_set_bit(decrypted, bit_pos, original_bit);

        pfx_shift_left(padded_prefix);
        pfx_set_bit(padded_prefix, 0, original_bit);
    }

    memcpy(out, decrypted, 16);
}

struct ipcrypt_implementation ipcrypt_armcrypto_implementation = {
    SODIUM_C99(.encrypt =) encrypt,         SODIUM_C99(.decrypt =) decrypt,
    SODIUM_C99(.nd_encrypt =) nd_encrypt,   SODIUM_C99(.nd_decrypt =) nd_decrypt,
    SODIUM_C99(.ndx_encrypt =) ndx_encrypt, SODIUM_C99(.ndx_decrypt =) ndx_decrypt,
    SODIUM_C99(.pfx_encrypt =) pfx_encrypt, SODIUM_C99(.pfx_decrypt =) pfx_decrypt
};

#    ifdef __clang__
#        pragma clang attribute pop
#    endif

#endif
