#include "crypto_core_keccak1600.h"
#include "ref/keccak1600_ref.h"

#ifdef __ARM_FEATURE_SHA3
# include "armsha3/keccak1600_armsha3.h"
# define keccak1600_init          keccak1600_armsha3_init
# define keccak1600_xor_bytes     keccak1600_armsha3_xor_bytes
# define keccak1600_extract_bytes keccak1600_armsha3_extract_bytes
# define keccak1600_permute_24    keccak1600_armsha3_permute_24
# define keccak1600_permute_12    keccak1600_armsha3_permute_12
#else
# define keccak1600_init          keccak1600_ref_init
# define keccak1600_xor_bytes     keccak1600_ref_xor_bytes
# define keccak1600_extract_bytes keccak1600_ref_extract_bytes
# define keccak1600_permute_24    keccak1600_ref_permute_24
# define keccak1600_permute_12    keccak1600_ref_permute_12
#endif

size_t
crypto_core_keccak1600_statebytes(void)
{
    return sizeof(crypto_core_keccak1600_state);
}

void
crypto_core_keccak1600_init(crypto_core_keccak1600_state *state)
{
    keccak1600_init(state->opaque);
}

void
crypto_core_keccak1600_xor_bytes(crypto_core_keccak1600_state *state,
                                 const unsigned char *bytes, size_t offset,
                                 size_t length)
{
    keccak1600_xor_bytes(state->opaque, bytes, offset, length);
}

void
crypto_core_keccak1600_extract_bytes(const crypto_core_keccak1600_state *state,
                                     unsigned char *bytes, size_t offset,
                                     size_t length)
{
    keccak1600_extract_bytes(state->opaque, bytes, offset, length);
}

void
crypto_core_keccak1600_permute_24(crypto_core_keccak1600_state *state)
{
    keccak1600_permute_24(state->opaque);
}

void
crypto_core_keccak1600_permute_12(crypto_core_keccak1600_state *state)
{
    keccak1600_permute_12(state->opaque);
}
