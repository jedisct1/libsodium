#ifndef core_h2c_H
#define core_h2c_H

#include "private/quirks.h"

#define CORE_H2C_SHA256 1
#define CORE_H2C_SHA512 2

int core_h2c_string_to_hash(unsigned char *h, const size_t h_len,
                            const unsigned char *ctx, size_t ctx_len,
                            const unsigned char *msg, size_t msg_len,
                            int hash_alg);
#endif
