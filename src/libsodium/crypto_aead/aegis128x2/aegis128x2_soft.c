#include <stddef.h>
#include <stdint.h>

#include "private/common.h"
#include "crypto_verify_16.h"
#include "crypto_verify_32.h"

#ifndef HAS_HW_AES

#include "private/softaes.h"
#include "crypto_aead_aegis128x2.h"
#include "aegis128x2_soft.h"
#include "export.h"
#include "utils.h"

#define AES_BLOCK_LENGTH 32

typedef struct {
    SoftAesBlock b0;
    SoftAesBlock b1;
} aes_block_t;

static inline aes_block_t
AES_BLOCK_XOR(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { softaes_block_xor(a.b0, b.b0), softaes_block_xor(a.b1, b.b1) };
}

static inline aes_block_t
AES_BLOCK_AND(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { softaes_block_and(a.b0, b.b0), softaes_block_and(a.b1, b.b1) };
}

static inline aes_block_t
AES_BLOCK_LOAD(const uint8_t *a)
{
    return (aes_block_t) { softaes_block_load(a), softaes_block_load(a + 16) };
}

static inline aes_block_t
AES_BLOCK_LOAD_64x2(uint64_t a, uint64_t b)
{
    const SoftAesBlock t = softaes_block_load64x2(a, b);
    return (aes_block_t) { t, t };
}
static inline void
AES_BLOCK_STORE(uint8_t *a, const aes_block_t b)
{
    softaes_block_store(a, b.b0);
    softaes_block_store(a + 16, b.b1);
}

static inline aes_block_t
AES_ENC(const aes_block_t a, const aes_block_t b)
{
    return (aes_block_t) { softaes_block_encrypt(a.b0, b.b0), softaes_block_encrypt(a.b1, b.b1) };
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

struct aegis128x2_implementation aegis128x2_soft_implementation = { SODIUM_C99(.encrypt_detached =)
                                                                      encrypt_detached,
                                                                  SODIUM_C99(.decrypt_detached =)
                                                                      decrypt_detached };

#endif
