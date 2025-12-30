#ifndef crypto_ipcrypt_H
#define crypto_ipcrypt_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
#    ifdef __GNUC__
#        pragma GCC diagnostic ignored "-Wlong-long"
#    endif
extern "C" {
#endif

#define crypto_ipcrypt_BYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_bytes(void);

#define crypto_ipcrypt_KEYBYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_keybytes(void);

#define crypto_ipcrypt_ND_KEYBYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_nd_keybytes(void);

#define crypto_ipcrypt_ND_TWEAKBYTES 8U
SODIUM_EXPORT
size_t crypto_ipcrypt_nd_tweakbytes(void);

#define crypto_ipcrypt_ND_INPUTBYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_nd_inputbytes(void);

#define crypto_ipcrypt_ND_OUTPUTBYTES 24U
SODIUM_EXPORT
size_t crypto_ipcrypt_nd_outputbytes(void);

#define crypto_ipcrypt_NDX_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_ipcrypt_ndx_keybytes(void);

#define crypto_ipcrypt_NDX_TWEAKBYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_ndx_tweakbytes(void);

#define crypto_ipcrypt_NDX_INPUTBYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_ndx_inputbytes(void);

#define crypto_ipcrypt_NDX_OUTPUTBYTES 32U
SODIUM_EXPORT
size_t crypto_ipcrypt_ndx_outputbytes(void);

#define crypto_ipcrypt_PFX_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_ipcrypt_pfx_keybytes(void);

#define crypto_ipcrypt_PFX_BYTES 16U
SODIUM_EXPORT
size_t crypto_ipcrypt_pfx_bytes(void);

SODIUM_EXPORT
void crypto_ipcrypt_keygen(unsigned char k[crypto_ipcrypt_KEYBYTES]) __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_ndx_keygen(unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_pfx_keygen(unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_encrypt(unsigned char       out[crypto_ipcrypt_BYTES],
                            const unsigned char in[crypto_ipcrypt_BYTES],
                            const unsigned char k[crypto_ipcrypt_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_decrypt(unsigned char       out[crypto_ipcrypt_BYTES],
                            const unsigned char in[crypto_ipcrypt_BYTES],
                            const unsigned char k[crypto_ipcrypt_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_nd_encrypt(unsigned char       out[crypto_ipcrypt_ND_OUTPUTBYTES],
                               const unsigned char in[crypto_ipcrypt_ND_INPUTBYTES],
                               const unsigned char t[crypto_ipcrypt_ND_TWEAKBYTES],
                               const unsigned char k[crypto_ipcrypt_ND_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_nd_decrypt(unsigned char       out[crypto_ipcrypt_ND_INPUTBYTES],
                               const unsigned char in[crypto_ipcrypt_ND_OUTPUTBYTES],
                               const unsigned char k[crypto_ipcrypt_ND_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_ndx_encrypt(unsigned char       out[crypto_ipcrypt_NDX_OUTPUTBYTES],
                                const unsigned char in[crypto_ipcrypt_NDX_INPUTBYTES],
                                const unsigned char t[crypto_ipcrypt_NDX_TWEAKBYTES],
                                const unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_ndx_decrypt(unsigned char       out[crypto_ipcrypt_NDX_INPUTBYTES],
                                const unsigned char in[crypto_ipcrypt_NDX_OUTPUTBYTES],
                                const unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_pfx_encrypt(unsigned char       out[crypto_ipcrypt_PFX_BYTES],
                                const unsigned char in[crypto_ipcrypt_PFX_BYTES],
                                const unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
void crypto_ipcrypt_pfx_decrypt(unsigned char       out[crypto_ipcrypt_PFX_BYTES],
                                const unsigned char in[crypto_ipcrypt_PFX_BYTES],
                                const unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
    __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif
