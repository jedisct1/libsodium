#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_aead_aegis256.h"
#include "crypto_verify_16.h"
#include "crypto_verify_32.h"
#include "export.h"
#include "utils.h"

#include "private/common.h"

#if defined(HAVE_ARMCRYPTO) && defined(NATIVE_LITTLE_ENDIAN)

#include "aegis256_armcrypto.h"

#ifndef __ARM_FEATURE_CRYPTO
#define __ARM_FEATURE_CRYPTO 1
#endif
#ifndef __ARM_FEATURE_AES
#define __ARM_FEATURE_AES 1
#endif

#include <arm_neon.h>

#ifdef __clang__
#pragma clang attribute push(__attribute__((target("neon,crypto,aes"))), apply_to = function)
#elif defined(__GNUC__)
#pragma GCC target("+simd+crypto")
#endif

#define AES_BLOCK_LENGTH 16

typedef uint8x16_t aes_block_t;
#define AES_BLOCK_XOR(A, B)       veorq_u8((A), (B))
#define AES_BLOCK_AND(A, B)       vandq_u8((A), (B))
#define AES_BLOCK_LOAD(A)         vld1q_u8(A)
#define AES_BLOCK_LOAD_64x2(A, B) vreinterpretq_u8_u64(vsetq_lane_u64((A), vmovq_n_u64(B), 1))
#define AES_BLOCK_STORE(A, B)     vst1q_u8((A), (B))
#define AES_ENC(A, B)             veorq_u8(vaesmcq_u8(vaeseq_u8((A), vmovq_n_u8(0))), (B))

static inline void
aegis256_update(aes_block_t *const state, const aes_block_t d)
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

#include "aegis256_common.h"

struct aegis256_implementation aegis256_armcrypto_implementation = { SODIUM_C99(.encrypt_detached =)
                                                                         encrypt_detached,
                                                                     SODIUM_C99(.decrypt_detached =)
                                                                         decrypt_detached };

#ifdef __clang__
#pragma clang attribute pop
#endif

#endif
