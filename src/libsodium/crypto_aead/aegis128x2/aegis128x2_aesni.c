#include <stddef.h>
#include <stdint.h>

#include "private/common.h"
#include "crypto_aead_aegis128x2.h"
#include "crypto_verify_16.h"
#include "crypto_verify_32.h"

#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)
#include "aegis128x2_aesni.h"

#ifdef __clang__
#    pragma clang attribute push(__attribute__((target("aes,avx"))), apply_to = function)
#elif defined(__GNUC__)
#    pragma GCC target("aes,avx")
#endif

#include <immintrin.h>
#include <wmmintrin.h>

#define AES_BLOCK_LENGTH 32

typedef struct {
    __m128i b0;
    __m128i b1;
} aes_block_t;

static inline aes_block_t
AES_BLOCK_XOR(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { _mm_xor_si128(a.b0, b.b0), _mm_xor_si128(a.b1, b.b1) };
}

static inline aes_block_t
AES_BLOCK_AND(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { _mm_and_si128(a.b0, b.b0), _mm_and_si128(a.b1, b.b1) };
}

static inline aes_block_t
AES_BLOCK_LOAD(const uint8_t *a)
{
    return (aes_block_t) { _mm_loadu_si128((const __m128i *) (const void *) a),
                           _mm_loadu_si128((const __m128i *) (const void *) (a + 16)) };
}

static inline aes_block_t
AES_BLOCK_LOAD_64x2(uint64_t a, uint64_t b)
{
    const __m128i t = _mm_set_epi64x((long long) a, (long long) b);
    return (aes_block_t) { t, t };
}

static inline void
AES_BLOCK_STORE(uint8_t *a, const aes_block_t b)
{
    _mm_storeu_si128((__m128i *) (void *) a, b.b0);
    _mm_storeu_si128((__m128i *) (void *) (a + 16), b.b1);
}

static inline aes_block_t
AES_ENC(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { _mm_aesenc_si128(a.b0, b.b0), _mm_aesenc_si128(a.b1, b.b1) };
}

static inline void
aegis128x2_update(aes_block_t *const state, const aes_block_t d1, const aes_block_t d2)
{
    aes_block_t tmp;

    tmp      = state[7];
    state[7] = AES_ENC(state[6], state[7]);
    state[6] = AES_ENC(state[5], state[6]);
    state[5] = AES_ENC(state[4], state[5]);
    state[4] = AES_ENC(state[3], state[4]);
    state[3] = AES_ENC(state[2], state[3]);
    state[2] = AES_ENC(state[1], state[2]);
    state[1] = AES_ENC(state[0], state[1]);
    state[0] = AES_ENC(tmp, state[0]);

    state[0] = AES_BLOCK_XOR(state[0], d1);
    state[4] = AES_BLOCK_XOR(state[4], d2);
}

#include "aegis128x2_common.h"

struct aegis128x2_implementation aegis128x2_aesni_implementation = {
    .encrypt_detached        = encrypt_detached,
    .decrypt_detached        = decrypt_detached,
};

#ifdef __clang__
#    pragma clang attribute pop
#endif

#endif
