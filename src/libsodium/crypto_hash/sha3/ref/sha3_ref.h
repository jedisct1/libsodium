#ifndef sha3_ref_H
#define sha3_ref_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_core_keccak1600.h"

#define SHA3_256_RATE 136
#define SHA3_512_RATE 72
#define SHA3_DOMAIN 0x06

typedef enum { SHA3_PHASE_ABSORBING = 0, SHA3_PHASE_FINALIZED = 1 } sha3_phase;

typedef struct sha3_state_internal_ {
    crypto_core_keccak1600_state state;
    size_t                       offset;
    size_t                       rate;
    size_t                       outlen;
    uint8_t                      phase;
} sha3_state_internal;

int sha3256_ref(unsigned char *out, const unsigned char *in, size_t inlen);
int sha3256_ref_init(sha3_state_internal *state);
int sha3256_ref_update(sha3_state_internal *state, const unsigned char *in, size_t inlen);
int sha3256_ref_final(sha3_state_internal *state, unsigned char *out);

int sha3512_ref(unsigned char *out, const unsigned char *in, size_t inlen);
int sha3512_ref_init(sha3_state_internal *state);
int sha3512_ref_update(sha3_state_internal *state, const unsigned char *in, size_t inlen);
int sha3512_ref_final(sha3_state_internal *state, unsigned char *out);

#endif
