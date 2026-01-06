#ifndef crypto_core_keccak1600_H
#define crypto_core_keccak1600_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#ifdef __IBMC__
# pragma pack(1)
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
# pragma pack(1)
#else
# pragma pack(push, 1)
#endif

typedef struct CRYPTO_ALIGN(16) crypto_core_keccak1600_state {
    unsigned char opaque[224];
} crypto_core_keccak1600_state;

#ifdef __IBMC__
# pragma pack(pop)
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
# pragma pack()
#else
# pragma pack(pop)
#endif

SODIUM_EXPORT
size_t crypto_core_keccak1600_statebytes(void);

SODIUM_EXPORT
void crypto_core_keccak1600_init(crypto_core_keccak1600_state *state)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_core_keccak1600_xor_bytes(crypto_core_keccak1600_state *state,
                                      const unsigned char *bytes,
                                      size_t offset, size_t length)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_core_keccak1600_extract_bytes(const crypto_core_keccak1600_state *state,
                                          unsigned char *bytes,
                                          size_t offset, size_t length)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_core_keccak1600_permute_24(crypto_core_keccak1600_state *state)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_core_keccak1600_permute_12(crypto_core_keccak1600_state *state)
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
