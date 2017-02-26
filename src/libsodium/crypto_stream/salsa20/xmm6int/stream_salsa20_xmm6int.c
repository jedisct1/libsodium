
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_EMMINTRIN_H) && defined(__x86_64__)

#if defined(HAVE_EMMINTRIN_H) || \
    (defined(_MSC_VER) &&        \
     (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)))
# ifdef __GNUC__
#  pragma GCC target("sse2")
# endif
# include <emmintrin.h>
#endif

#include "crypto_stream_salsa20.h"
#include "private/common.h"
#include "utils.h"

#include "../stream_salsa20.h"
#include "stream_salsa20_xmm6int.h"

#define ROUNDS 20

typedef struct salsa_ctx {
    uint32_t input[16];
} salsa_ctx;

static const int TR[16] = {
    0, 5, 10, 15, 12, 1, 6, 11, 8, 13, 2, 7, 4, 9, 14, 3
};

static void
salsa20_wordtobyte_tr(uint8_t output[64], const uint32_t input[16])
{
    uint32_t x[16];
    int      i;

    for (i = 0; i < 16; i++) {
        x[TR[i]] = input[TR[i]];
    }
    for (i = 20; i > 0; i -= 2) {
        x[TR[4]]  ^= ROTL32(x[TR[0]]  + x[TR[12]], 7);
        x[TR[8]]  ^= ROTL32(x[TR[4]]  + x[TR[0]], 9);
        x[TR[12]] ^= ROTL32(x[TR[8]]  + x[TR[4]], 13);
        x[TR[0]]  ^= ROTL32(x[TR[12]] + x[TR[8]], 18);
        x[TR[9]]  ^= ROTL32(x[TR[5]]  + x[TR[1]], 7);
        x[TR[13]] ^= ROTL32(x[TR[9]]  + x[TR[5]], 9);
        x[TR[1]]  ^= ROTL32(x[TR[13]] + x[TR[9]], 13);
        x[TR[5]]  ^= ROTL32(x[TR[1]]  + x[TR[13]], 18);
        x[TR[14]] ^= ROTL32(x[TR[10]] + x[TR[6]], 7);
        x[TR[2]]  ^= ROTL32(x[TR[14]] + x[TR[10]], 9);
        x[TR[6]]  ^= ROTL32(x[TR[2]]  + x[TR[14]], 13);
        x[TR[10]] ^= ROTL32(x[TR[6]]  + x[TR[2]], 18);
        x[TR[3]]  ^= ROTL32(x[TR[15]] + x[TR[11]], 7);
        x[TR[7]]  ^= ROTL32(x[TR[3]]  + x[TR[15]], 9);
        x[TR[11]] ^= ROTL32(x[TR[7]]  + x[TR[3]], 13);
        x[TR[15]] ^= ROTL32(x[TR[11]] + x[TR[7]], 18);
        x[TR[1]]  ^= ROTL32(x[TR[0]]  + x[TR[3]], 7);
        x[TR[2]]  ^= ROTL32(x[TR[1]]  + x[TR[0]], 9);
        x[TR[3]]  ^= ROTL32(x[TR[2]]  + x[TR[1]], 13);
        x[TR[0]]  ^= ROTL32(x[TR[3]]  + x[TR[2]], 18);
        x[TR[6]]  ^= ROTL32(x[TR[5]]  + x[TR[4]], 7);
        x[TR[7]]  ^= ROTL32(x[TR[6]]  + x[TR[5]], 9);
        x[TR[4]]  ^= ROTL32(x[TR[7]]  + x[TR[6]], 13);
        x[TR[5]]  ^= ROTL32(x[TR[4]]  + x[TR[7]], 18);
        x[TR[11]] ^= ROTL32(x[TR[10]] + x[TR[9]], 7);
        x[TR[8]]  ^= ROTL32(x[TR[11]] + x[TR[10]], 9);
        x[TR[9]]  ^= ROTL32(x[TR[8]]  + x[TR[11]], 13);
        x[TR[10]] ^= ROTL32(x[TR[9]]  + x[TR[8]], 18);
        x[TR[12]] ^= ROTL32(x[TR[15]] + x[TR[14]], 7);
        x[TR[13]] ^= ROTL32(x[TR[12]] + x[TR[15]], 9);
        x[TR[14]] ^= ROTL32(x[TR[13]] + x[TR[12]], 13);
        x[TR[15]] ^= ROTL32(x[TR[14]] + x[TR[13]], 18);
    }
    for (i = 0; i < 16; i++) {
        x[TR[i]] += input[TR[i]];
    }
    for (i = 0; i < 16; i++) {
        STORE32_LE(output + 4 * i, x[TR[i]]);
    }
}

static void
salsa_keysetup(salsa_ctx *ctx, const uint8_t *k)
{
    ctx->input[TR[1]]  = LOAD32_LE(k + 0);
    ctx->input[TR[2]]  = LOAD32_LE(k + 4);
    ctx->input[TR[3]]  = LOAD32_LE(k + 8);
    ctx->input[TR[4]]  = LOAD32_LE(k + 12);
    ctx->input[TR[11]] = LOAD32_LE(k + 16);
    ctx->input[TR[12]] = LOAD32_LE(k + 20);
    ctx->input[TR[13]] = LOAD32_LE(k + 24);
    ctx->input[TR[14]] = LOAD32_LE(k + 28);
    ctx->input[TR[0]]  = 0x61707865;
    ctx->input[TR[5]]  = 0x3320646e;
    ctx->input[TR[10]] = 0x79622d32;
    ctx->input[TR[15]] = 0x6b206574;
}

static void
salsa_ivsetup(salsa_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[TR[6]] = LOAD32_LE(iv + 0);
    ctx->input[TR[7]] = LOAD32_LE(iv + 4);
    ctx->input[TR[8]] = counter == NULL ? 0 : LOAD32_LE(counter + 0);
    ctx->input[TR[9]] = counter == NULL ? 0 : LOAD32_LE(counter + 4);
}

static void
salsa20_encrypt_bytes(salsa_ctx *ctx, const uint8_t *m, uint8_t *c,
                      unsigned long long bytes)
{
    uint8_t partialblock[64];
    uint32_t * const x = &ctx->input[0];
    int     i;

    if (!bytes) {
        return; /* LCOV_EXCL_LINE */
    }
    if (bytes > 64ULL * (1ULL << 32) - 64ULL) {
        abort();
    }

#include "u4.h"
#include "u1.h"

    if (!bytes) {
        return;
    }
    salsa20_wordtobyte_tr(partialblock, x);
    for (i = 0; i < bytes; i++) {
        c[i] = m[i] ^ partialblock[i];
    }
}

static int
stream_ref(unsigned char *c, unsigned long long clen, const unsigned char *n,
           const unsigned char *k)
{
    struct salsa_ctx ctx;

    if (!clen) {
        return 0;
    }
    COMPILER_ASSERT(crypto_stream_salsa20_KEYBYTES == 256 / 8);
    salsa_keysetup(&ctx, k);
    salsa_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    salsa20_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ref_xor_ic(unsigned char *c, const unsigned char *m,
                  unsigned long long mlen, const unsigned char *n, uint64_t ic,
                  const unsigned char *k)
{
    struct salsa_ctx ctx;
    uint8_t           ic_bytes[8];
    uint32_t          ic_high;
    uint32_t          ic_low;

    if (!mlen) {
        return 0;
    }
    ic_high = (uint32_t) (ic >> 32);
    ic_low  = (uint32_t) (ic);
    STORE32_LE(&ic_bytes[0], ic_low);
    STORE32_LE(&ic_bytes[4], ic_high);
    salsa_keysetup(&ctx, k);
    salsa_ivsetup(&ctx, n, ic_bytes);
    salsa20_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

struct crypto_stream_salsa20_implementation
    crypto_stream_salsa20_xmm6int_implementation = {
        SODIUM_C99(.stream =) stream_ref,
        SODIUM_C99(.stream_xor_ic =) stream_ref_xor_ic
    };

#endif
