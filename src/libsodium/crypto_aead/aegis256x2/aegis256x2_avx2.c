#include <stddef.h>
#include <stdint.h>

#include "private/common.h"
#include "private/implementations.h"
#include "crypto_aead_aegis256x2.h"
#include "crypto_verify_16.h"
#include "crypto_verify_32.h"

#include "utils.h"

#if defined(HAVE_AVX2INTRIN_H) && defined(HAVE_WMMINTRIN_H)
#include "aegis256x2_avx2.h"

#ifdef __clang__
#pragma clang attribute push(__attribute__((target("vaes,avx2"))), apply_to = function)
#elif defined(__GNUC__)
#pragma GCC target("vaes,avx2")
#endif

#include <immintrin.h>

#define AES_BLOCK_LENGTH 32

typedef __m256i aes_block_t;

#define AES_BLOCK_XOR(A, B) _mm256_xor_si256((A), (B))
#define AES_BLOCK_AND(A, B) _mm256_and_si256((A), (B))
#define AES_BLOCK_LOAD128_BROADCAST(A) \
            _mm256_broadcastsi128_si256(_mm_loadu_si128((const void *) (A)))
#define AES_BLOCK_LOAD(A)         _mm256_loadu_si256((const aes_block_t *) (const void *) (A))
#define AES_BLOCK_LOAD_64x2(A, B) _mm256_broadcastsi128_si256(_mm_set_epi64x((A), (B)))
#define AES_BLOCK_STORE(A, B)     _mm256_storeu_si256((aes_block_t *) (void *) (A), (B))
#define AES_ENC(A, B)             _mm256_aesenc_epi128((A), (B))

static inline void
aegis256x2_update(aes_block_t *const state, const aes_block_t d)
{
    aes_block_t tmp;

    tmp      = state[5];
    state[5] = AES_ENC(state[4], state[5]);
    state[4] = AES_ENC(state[3], state[4]);
    state[3] = AES_ENC(state[2], state[3]);
    state[2] = AES_ENC(state[1], state[2]);
    state[1] = AES_ENC(state[0], state[1]);
    state[0] = AES_BLOCK_XOR(AES_ENC(tmp, state[0]), d);
}

#include "aegis256x2_common.h"

struct aegis256x2_implementation aegis256x2_avx2_implementation = { SODIUM_C99(.encrypt_detached =)
                                                                      encrypt_detached,
                                                                  SODIUM_C99(.decrypt_detached =)
                                                                      decrypt_detached };

#ifdef __clang__
#pragma clang attribute pop
#endif

#endif
