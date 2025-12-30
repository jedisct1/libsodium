#include "crypto_core_keccak1600.h"
#include "ref/keccak1600_ref.h"

size_t
crypto_core_keccak1600_statebytes(void)
{
    return crypto_core_keccak1600_STATEBYTES;
}

void
crypto_core_keccak1600_init(void *state)
{
    keccak1600_ref_init(state);
}

void
crypto_core_keccak1600_xor_bytes(void *state, const unsigned char *bytes, size_t offset,
                                 size_t length)
{
    keccak1600_ref_xor_bytes(state, bytes, offset, length);
}

void
crypto_core_keccak1600_extract_bytes(const void *state, unsigned char *bytes, size_t offset,
                                     size_t length)
{
    keccak1600_ref_extract_bytes(state, bytes, offset, length);
}

void
crypto_core_keccak1600_permute_24(void *state)
{
    keccak1600_ref_permute_24(state);
}

void
crypto_core_keccak1600_permute_12(void *state)
{
    keccak1600_ref_permute_12(state);
}
