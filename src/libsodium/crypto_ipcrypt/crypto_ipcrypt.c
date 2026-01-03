
#include <stdlib.h>

#include "core.h"
#include "crypto_ipcrypt.h"
#include "private/common.h"
#include "private/implementations.h"
#include "randombytes.h"
#include "runtime.h"

#include "ipcrypt_soft.h"

#if defined(HAVE_ARMCRYPTO) && defined(NATIVE_LITTLE_ENDIAN)
#    include "ipcrypt_armcrypto.h"
#endif

#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)
#    include "ipcrypt_aesni.h"
#endif

static const ipcrypt_implementation *implementation = &ipcrypt_soft_implementation;

size_t
crypto_ipcrypt_bytes(void)
{
    return crypto_ipcrypt_BYTES;
}

size_t
crypto_ipcrypt_keybytes(void)
{
    return crypto_ipcrypt_KEYBYTES;
}

size_t
crypto_ipcrypt_nd_keybytes(void)
{
    return crypto_ipcrypt_ND_KEYBYTES;
}

size_t
crypto_ipcrypt_nd_tweakbytes(void)
{
    return crypto_ipcrypt_ND_TWEAKBYTES;
}

size_t
crypto_ipcrypt_nd_inputbytes(void)
{
    return crypto_ipcrypt_ND_INPUTBYTES;
}

size_t
crypto_ipcrypt_nd_outputbytes(void)
{
    return crypto_ipcrypt_ND_OUTPUTBYTES;
}

size_t
crypto_ipcrypt_ndx_keybytes(void)
{
    return crypto_ipcrypt_NDX_KEYBYTES;
}

size_t
crypto_ipcrypt_ndx_tweakbytes(void)
{
    return crypto_ipcrypt_NDX_TWEAKBYTES;
}

size_t
crypto_ipcrypt_ndx_inputbytes(void)
{
    return crypto_ipcrypt_NDX_INPUTBYTES;
}

size_t
crypto_ipcrypt_ndx_outputbytes(void)
{
    return crypto_ipcrypt_NDX_OUTPUTBYTES;
}

size_t
crypto_ipcrypt_pfx_keybytes(void)
{
    return crypto_ipcrypt_PFX_KEYBYTES;
}

size_t
crypto_ipcrypt_pfx_bytes(void)
{
    return crypto_ipcrypt_PFX_BYTES;
}

void
crypto_ipcrypt_keygen(unsigned char k[crypto_ipcrypt_KEYBYTES])
{
    randombytes_buf(k, crypto_ipcrypt_KEYBYTES);
}

void
crypto_ipcrypt_ndx_keygen(unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
{
    randombytes_buf(k, crypto_ipcrypt_NDX_KEYBYTES);
}

void
crypto_ipcrypt_pfx_keygen(unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
{
    randombytes_buf(k, crypto_ipcrypt_PFX_KEYBYTES);
}

void
crypto_ipcrypt_encrypt(unsigned char       out[crypto_ipcrypt_BYTES],
                       const unsigned char in[crypto_ipcrypt_BYTES],
                       const unsigned char k[crypto_ipcrypt_KEYBYTES])
{
    implementation->encrypt(out, in, k);
}

void
crypto_ipcrypt_decrypt(unsigned char       out[crypto_ipcrypt_BYTES],
                       const unsigned char in[crypto_ipcrypt_BYTES],
                       const unsigned char k[crypto_ipcrypt_KEYBYTES])
{
    implementation->decrypt(out, in, k);
}

void
crypto_ipcrypt_nd_encrypt(unsigned char       out[crypto_ipcrypt_ND_OUTPUTBYTES],
                          const unsigned char in[crypto_ipcrypt_ND_INPUTBYTES],
                          const unsigned char t[crypto_ipcrypt_ND_TWEAKBYTES],
                          const unsigned char k[crypto_ipcrypt_ND_KEYBYTES])
{
    implementation->nd_encrypt(out, in, t, k);
}

void
crypto_ipcrypt_nd_decrypt(unsigned char       out[crypto_ipcrypt_ND_INPUTBYTES],
                          const unsigned char in[crypto_ipcrypt_ND_OUTPUTBYTES],
                          const unsigned char k[crypto_ipcrypt_ND_KEYBYTES])
{
    implementation->nd_decrypt(out, in, k);
}

void
crypto_ipcrypt_ndx_encrypt(unsigned char       out[crypto_ipcrypt_NDX_OUTPUTBYTES],
                           const unsigned char in[crypto_ipcrypt_NDX_INPUTBYTES],
                           const unsigned char t[crypto_ipcrypt_NDX_TWEAKBYTES],
                           const unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
{
    implementation->ndx_encrypt(out, in, t, k);
}

void
crypto_ipcrypt_ndx_decrypt(unsigned char       out[crypto_ipcrypt_NDX_INPUTBYTES],
                           const unsigned char in[crypto_ipcrypt_NDX_OUTPUTBYTES],
                           const unsigned char k[crypto_ipcrypt_NDX_KEYBYTES])
{
    implementation->ndx_decrypt(out, in, k);
}

void
crypto_ipcrypt_pfx_encrypt(unsigned char       out[crypto_ipcrypt_PFX_BYTES],
                           const unsigned char in[crypto_ipcrypt_PFX_BYTES],
                           const unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
{
    implementation->pfx_encrypt(out, in, k);
}

void
crypto_ipcrypt_pfx_decrypt(unsigned char       out[crypto_ipcrypt_PFX_BYTES],
                           const unsigned char in[crypto_ipcrypt_PFX_BYTES],
                           const unsigned char k[crypto_ipcrypt_PFX_KEYBYTES])
{
    implementation->pfx_decrypt(out, in, k);
}

int
_crypto_ipcrypt_pick_best_implementation(void)
{
    implementation = &ipcrypt_soft_implementation;

#if defined(HAVE_ARMCRYPTO) && defined(NATIVE_LITTLE_ENDIAN)
    if (sodium_runtime_has_armcrypto()) {
        implementation = &ipcrypt_armcrypto_implementation;
        return 0;
    }
#endif

#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)
    if (sodium_runtime_has_aesni()) {
        implementation = &ipcrypt_aesni_implementation;
        return 0;
    }
#endif
    return 0;
}
