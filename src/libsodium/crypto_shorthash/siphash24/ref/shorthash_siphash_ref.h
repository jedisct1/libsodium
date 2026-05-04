#ifndef shorthash_siphash_H
#define shorthash_siphash_H

#include "private/common.h"

#define SIPROUND             \
    do {                     \
        v0 += v1;            \
        v2 += v3;            \
        v1 = ROTL64(v1, 13); \
        v3 = ROTL64(v3, 16); \
        v1 ^= v0;            \
        v3 ^= v2;            \
        v0 = ROTL64(v0, 32); \
        v0 += v3;            \
        v2 += v1;            \
        v3 = ROTL64(v3, 21); \
        v1 = ROTL64(v1, 17); \
        v3 ^= v0;            \
        v1 ^= v2;            \
        v2 = ROTL64(v2, 32); \
    } while (0)

#endif
