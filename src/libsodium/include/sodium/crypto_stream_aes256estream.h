#ifndef crypto_stream_aes256estream_H
#define crypto_stream_aes256estream_H

/*
 * WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 * While it provides some protection against eavesdropping, it does NOT
 * provide any security against active attacks.
 * Furthermore, this implementation was not part of NaCl.
 * 
 * If you are looking for a stream cipher, you might consider
 * crypto_stream_aes128ctr, crypto_stream_chacha20 or crypto_stream_(x)salsa20
 * which are timing-attack resistant.
 * 
 * But unless you know what you're doing, what you are looking for is probably
 * the crypto_box or crypto_secretbox functions.
 */

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_stream_aes256estream_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_stream_aes256estream_keybytes(void);

#define crypto_stream_aes256estream_NONCEBYTES 16U
SODIUM_EXPORT
size_t crypto_stream_aes256estream_noncebytes(void);

#define crypto_stream_aes256estream_BEFORENMBYTES 276U
SODIUM_EXPORT
size_t crypto_stream_aes256estream_beforenmbytes(void);

SODIUM_EXPORT
int crypto_stream_aes256estream(unsigned char *out, unsigned long long len,
                                const unsigned char *nonce, const unsigned char *c);

SODIUM_EXPORT
int crypto_stream_aes256estream_xor(unsigned char *out, const unsigned char *in,
                                    unsigned long long inlen, const unsigned char *n,
                                    const unsigned char *k);

SODIUM_EXPORT
int crypto_stream_aes256estream_beforenm(unsigned char *c, const unsigned char *k);

SODIUM_EXPORT
int crypto_stream_aes256estream_afternm(unsigned char *out, unsigned long long len,
                                        const unsigned char *nonce,
                                        const unsigned char *c);

SODIUM_EXPORT
int crypto_stream_aes256estream_xor_afternm(unsigned char *out, const unsigned char *in,
                                            unsigned long long len,
                                            const unsigned char *nonce,
                                            const unsigned char *c);

#ifdef __cplusplus
}
#endif

#endif
