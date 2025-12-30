#ifndef keccak1600_ref_H
#define keccak1600_ref_H

#include <stddef.h>

#include "private/quirks.h"

void keccak1600_ref_init(void *state);

void keccak1600_ref_xor_bytes(void *state, const unsigned char *bytes,
                              size_t offset, size_t length);

void keccak1600_ref_extract_bytes(const void *state, unsigned char *bytes,
                                  size_t offset, size_t length);

void keccak1600_ref_permute_24(void *state);

void keccak1600_ref_permute_12(void *state);

#endif /* keccak1600_ref_H */
