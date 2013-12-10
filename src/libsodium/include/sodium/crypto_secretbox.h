#ifndef crypto_secretbox_H
#define crypto_secretbox_H

#include <stddef.h>

#include "crypto_secretbox_xsalsa20poly1305.h"
#include "export.h"

/**
 * \defgroup asymenc Authenticated Symmetric Encryption
 *
 * Definitions and functions for authenticated symmetric encryption.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_secretbox_KEYBYTES crypto_secretbox_xsalsa20poly1305_KEYBYTES
/// Size of symmetric key.
SODIUM_EXPORT
size_t  crypto_secretbox_keybytes(void);

#define crypto_secretbox_NONCEBYTES crypto_secretbox_xsalsa20poly1305_NONCEBYTES
/// Size of the nonce.
SODIUM_EXPORT
size_t  crypto_secretbox_noncebytes(void);

#define crypto_secretbox_ZEROBYTES crypto_secretbox_xsalsa20poly1305_ZEROBYTES
/// No. of leading 0 bytes in the message.
SODIUM_EXPORT
size_t  crypto_secretbox_zerobytes(void);

#define crypto_secretbox_BOXZEROBYTES crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES
/// No. of leading 0 bytes in the cipher-text.
SODIUM_EXPORT
size_t  crypto_secretbox_boxzerobytes(void);

#define crypto_secretbox_MACBYTES crypto_secretbox_xsalsa20poly1305_MACBYTES
///
SODIUM_EXPORT
size_t  crypto_secretbox_macbytes(void);

#define crypto_secretbox_PRIMITIVE "xsalsa20poly1305"
///
SODIUM_EXPORT
const char *crypto_secretbox_primitive(void);

/**
 *
 * Encrypts and authenticates a message using the given secret key, and nonce..
 *
 * @param[out] ctxt   the buffer for the cipher-text.
 * @param[in]  msg    the message to be encrypted.
 * @param[in]  mlen   the length of msg.
 * @param[in]  nonce  a nonce with length crypto_box_NONCEBYTES.
 * @param[in]  key    the shared secret key.
 *
 * @return 0 if operation is successful.
 *
 * @pre  first crypto_secretbox_ZEROBYTES of msg be all 0..
 * @post first crypto_secretbox_BOXZERBYTES of ctxt be all 0.
 * @post first mlen bytes of ctxt will contain the ciphertext.
 *
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_secretbox_KEYBYTES];
 * const unsigned char n[crypto_secretbox_NONCEBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * unsigned char c[...]; unsigned long long clen;
 *
 * crypto_secretbox(c,m,mlen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_secretbox(unsigned char *c, const unsigned char *m,
                     unsigned long long mlen, const unsigned char *n,
                     const unsigned char *k);

/**
 *
 * Decrypts a ciphertext ctxt given the receivers private key, and
 * senders public key.
 *
 * @param[out] msg    the buffer to place resulting plaintext.
 * @param[in]  ctxt   the ciphertext to be decrypted.
 * @param[in]  clen   the length of the ciphertext.
 * @param[in]  nonce  a randomly generated nonce.
 * @param[in]  key    the shared secret key.
 *
 * @return 0 if successful and -1 if verification fails.
 *
 * @pre  first crypto_secretbox_BOXZEROBYTES of ctxt be all 0.
 * @pre  the nonce must be of length crypto_secretbox_NONCEBYTES
 * @post first clen bytes of msg will contain the plaintext.
 * @post first crypto_secretbox_ZEROBYTES of msg will be all 0.
 *
 * @warning if verification fails msg may contain data from the
 * computation.

 * Example innvocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_secretbox_KEYBYTES];
 * const unsigned char n[crypto_secretbox_NONCEBYTES];
 * const unsigned char c[...]; unsigned long long clen;
 * unsigned char m[...];
 *
 * crypto_secretbox_open(m,c,clen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_secretbox_open(unsigned char *m, const unsigned char *c,
                          unsigned long long clen, const unsigned char *n,
                          const unsigned char *k);


/** < @} */

#ifdef __cplusplus
}
#endif

#endif
