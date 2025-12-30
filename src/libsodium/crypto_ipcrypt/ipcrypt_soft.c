#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_ipcrypt.h"
#include "utils.h"

#include "private/common.h"
#include "private/softaes.h"

#include "ipcrypt_soft.h"

#define ROUNDS 10

typedef SoftAesBlock aes_block_t;
#define AES_BLOCK_XOR(A, B)       softaes_block_xor((A), (B))
#define AES_BLOCK_AND(A, B)       softaes_block_and((A), (B))
#define AES_BLOCK_LOAD(A)         softaes_block_load(A)
#define AES_BLOCK_LOAD_64x2(A, B) softaes_block_load64x2((A), (B))
#define AES_BLOCK_STORE(A, B)     softaes_block_store((A), (B))
#define AES_ENC(A, B)             softaes_block_encrypt((A), (B))
#define AES_ENCLAST(A, B)         softaes_block_encryptlast((A), (B))
#define AES_DEC(A, B)             softaes_block_decrypt((A), (B))
#define AES_DECLAST(A, B)         softaes_block_decryptlast((A), (B))
#define AES_INV_MIX(A)            softaes_inv_mix_columns((A))

typedef aes_block_t KeySchedule[1 + ROUNDS];

static void
expand_key(KeySchedule rkeys, const uint8_t key[16])
{
    softaes_expand_key128(rkeys, key);
}

static void
aes_encrypt(uint8_t out[16], const uint8_t in[16], const KeySchedule rkeys)
{
    aes_block_t t;
    size_t      i;

    t = AES_BLOCK_XOR(AES_BLOCK_LOAD(in), rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENC(t, rkeys[i]);
    }
    t = AES_ENCLAST(t, rkeys[ROUNDS]);
    AES_BLOCK_STORE(out, t);
}

static void
aes_decrypt(uint8_t out[16], const uint8_t in[16], const KeySchedule rkeys)
{
    KeySchedule rkeys_inv;
    aes_block_t t;
    size_t      i;

    for (i = 0; i <= ROUNDS; i++) {
        rkeys_inv[i] = rkeys[i];
    }
    softaes_invert_key_schedule128(rkeys_inv);

    t = AES_BLOCK_XOR(AES_BLOCK_LOAD(in), rkeys_inv[ROUNDS]);
    for (i = ROUNDS - 1; i > 0; i--) {
        t = AES_DEC(t, rkeys_inv[i]);
    }
    t = AES_DECLAST(t, rkeys_inv[0]);
    AES_BLOCK_STORE(out, t);
}

static aes_block_t
tweak_expand(const uint8_t tweak[8])
{
    aes_block_t out;

    out.w0 = ((uint32_t) tweak[0]) | ((uint32_t) tweak[1] << 8);
    out.w1 = ((uint32_t) tweak[2]) | ((uint32_t) tweak[3] << 8);
    out.w2 = ((uint32_t) tweak[4]) | ((uint32_t) tweak[5] << 8);
    out.w3 = ((uint32_t) tweak[6]) | ((uint32_t) tweak[7] << 8);

    return out;
}

static void
aes_encrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const KeySchedule rkeys)
{
    const aes_block_t tweak_block = tweak_expand(tweak);
    aes_block_t       t;
    size_t            i;

    t = AES_BLOCK_XOR(AES_BLOCK_XOR(AES_BLOCK_LOAD(in), tweak_block), rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENC(t, AES_BLOCK_XOR(tweak_block, rkeys[i]));
    }
    t = AES_ENCLAST(t, AES_BLOCK_XOR(tweak_block, rkeys[ROUNDS]));
    AES_BLOCK_STORE(out, t);
}

static void
aes_decrypt_with_tweak(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[8],
                       const KeySchedule rkeys)
{
    KeySchedule       rkeys_inv;
    const aes_block_t tweak_block     = tweak_expand(tweak);
    const aes_block_t tweak_block_inv = AES_INV_MIX(tweak_block);
    aes_block_t       t;
    size_t            i;

    for (i = 0; i <= ROUNDS; i++) {
        rkeys_inv[i] = rkeys[i];
    }
    softaes_invert_key_schedule128(rkeys_inv);

    t = AES_BLOCK_XOR(AES_BLOCK_XOR(AES_BLOCK_LOAD(in), tweak_block), rkeys_inv[ROUNDS]);
    for (i = ROUNDS - 1; i > 0; i--) {
        t = AES_DEC(t, AES_BLOCK_XOR(tweak_block_inv, rkeys_inv[i]));
    }
    t = AES_DECLAST(t, AES_BLOCK_XOR(tweak_block, rkeys_inv[0]));
    AES_BLOCK_STORE(out, t);
}

static aes_block_t
aes_xex_tweak(const uint8_t tweak[16], const KeySchedule tkeys)
{
    aes_block_t tt;
    size_t      i;

    tt = AES_BLOCK_XOR(AES_BLOCK_LOAD(tweak), tkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        tt = AES_ENC(tt, tkeys[i]);
    }
    tt = AES_ENCLAST(tt, tkeys[ROUNDS]);
    return tt;
}

static void
aes_xex_encrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const KeySchedule tkeys, const KeySchedule rkeys)
{
    const aes_block_t tt = aes_xex_tweak(tweak, tkeys);
    aes_block_t       t;
    size_t            i;

    t = AES_BLOCK_XOR(AES_BLOCK_XOR(AES_BLOCK_LOAD(in), tt), rkeys[0]);
    for (i = 1; i < ROUNDS; i++) {
        t = AES_ENC(t, rkeys[i]);
    }
    t = AES_ENCLAST(t, AES_BLOCK_XOR(rkeys[ROUNDS], tt));
    AES_BLOCK_STORE(out, t);
}

static void
aes_xex_decrypt(uint8_t out[16], const uint8_t in[16], const uint8_t tweak[16],
                const KeySchedule tkeys, const KeySchedule rkeys)
{
    KeySchedule       rkeys_inv;
    const aes_block_t tt = aes_xex_tweak(tweak, tkeys);
    aes_block_t       t;
    size_t            i;

    for (i = 0; i <= ROUNDS; i++) {
        rkeys_inv[i] = rkeys[i];
    }
    softaes_invert_key_schedule128(rkeys_inv);

    t = AES_BLOCK_XOR(AES_BLOCK_XOR(AES_BLOCK_LOAD(in), tt), rkeys_inv[ROUNDS]);
    for (i = ROUNDS - 1; i > 0; i--) {
        t = AES_DEC(t, rkeys_inv[i]);
    }
    t = AES_DECLAST(t, AES_BLOCK_XOR(rkeys_inv[0], tt));
    AES_BLOCK_STORE(out, t);
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

    AES_BLOCK_STORE(diff, AES_BLOCK_XOR(tkeys[ROUNDS / 2], rkeys[ROUNDS / 2]));
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

    AES_BLOCK_STORE(diff, AES_BLOCK_XOR(tkeys[ROUNDS / 2], rkeys[ROUNDS / 2]));
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

#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("" : "+r"(mask)::);
#endif
    ip16[byte_index] = (ip16[byte_index] & ~bit_mask) | (bit_mask & mask);
}

static void
pfx_shift_left(uint8_t ip16[16])
{
    size_t i;

    for (i = 0; i < 15; i++) {
        ip16[i] = (ip16[i] << 1) | (ip16[i + 1] >> 7);
    }
    ip16[15] <<= 1;
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
    aes_block_t  e1, e2, e;
    unsigned int prefix_start = 0;
    unsigned int prefix_len_bits;
    unsigned int bit_pos;
    uint8_t      cipher_bit;
    uint8_t      original_bit;
    size_t       i;
    uint8_t      d;

    expand_key(k1keys, k);
    expand_key(k2keys, k + 16);

    AES_BLOCK_STORE(diff, AES_BLOCK_XOR(k1keys[ROUNDS / 2], k2keys[ROUNDS / 2]));
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
        e1 = AES_BLOCK_XOR(AES_BLOCK_LOAD(padded_prefix), k1keys[0]);
        e2 = AES_BLOCK_XOR(AES_BLOCK_LOAD(padded_prefix), k2keys[0]);
        for (i = 1; i < ROUNDS; i++) {
            e1 = AES_ENC(e1, k1keys[i]);
            e2 = AES_ENC(e2, k2keys[i]);
        }
        e1 = AES_ENCLAST(e1, k1keys[ROUNDS]);
        e2 = AES_ENCLAST(e2, k2keys[ROUNDS]);

        e = AES_BLOCK_XOR(e1, e2);
        AES_BLOCK_STORE(t, e);

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
    aes_block_t  e1, e2, e;
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

    AES_BLOCK_STORE(diff, AES_BLOCK_XOR(k1keys[ROUNDS / 2], k2keys[ROUNDS / 2]));
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
        e1 = AES_BLOCK_XOR(AES_BLOCK_LOAD(padded_prefix), k1keys[0]);
        e2 = AES_BLOCK_XOR(AES_BLOCK_LOAD(padded_prefix), k2keys[0]);
        for (i = 1; i < ROUNDS; i++) {
            e1 = AES_ENC(e1, k1keys[i]);
            e2 = AES_ENC(e2, k2keys[i]);
        }
        e1 = AES_ENCLAST(e1, k1keys[ROUNDS]);
        e2 = AES_ENCLAST(e2, k2keys[ROUNDS]);

        e = AES_BLOCK_XOR(e1, e2);
        AES_BLOCK_STORE(t, e);

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

struct ipcrypt_implementation ipcrypt_soft_implementation = {
    SODIUM_C99(.encrypt =) encrypt,         SODIUM_C99(.decrypt =) decrypt,
    SODIUM_C99(.nd_encrypt =) nd_encrypt,   SODIUM_C99(.nd_decrypt =) nd_decrypt,
    SODIUM_C99(.ndx_encrypt =) ndx_encrypt, SODIUM_C99(.ndx_decrypt =) ndx_decrypt,
    SODIUM_C99(.pfx_encrypt =) pfx_encrypt, SODIUM_C99(.pfx_decrypt =) pfx_decrypt
};
