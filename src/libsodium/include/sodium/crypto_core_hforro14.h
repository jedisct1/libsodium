#ifndef crypto_core_hforro14_H
#define crypto_core_hforro14_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define crypto_core_hforro14_OUTPUTBYTES 32U
    SODIUM_EXPORT
    size_t crypto_core_hforro14_outputbytes(void);

#define crypto_core_hforro14_INPUTBYTES 16U
    SODIUM_EXPORT
    size_t crypto_core_hforro14_inputbytes(void);

#define crypto_core_hforro14_KEYBYTES 32U
    SODIUM_EXPORT
    size_t crypto_core_hforro14_keybytes(void);

#define crypto_core_hforro14_CONSTBYTES 16U
    SODIUM_EXPORT
    size_t crypto_core_hforro14_constbytes(void);

    SODIUM_EXPORT
    int crypto_core_hforro14(unsigned char *out, const unsigned char *in,
                             const unsigned char *k, const unsigned char *c)
        __attribute__((nonnull(1, 2, 3)));

#ifdef __cplusplus
}
#endif

#endif
