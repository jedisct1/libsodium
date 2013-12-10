#ifndef crypto_stream_H
#define crypto_stream_H

/** \defgroup symenc Symmetric Encryption
 *
 * Definitions and functions for symmetric encryption.
 *
 * Alongside the standard interface there also exists a
 * pre-computation interface. In the event that applications are
 * required to send several messages to the same receiver, speed can
 * be gained by splitting the operation into two steps: before and
 * after. Similarly applications that receive several messages from
 * the same sender can gain speed through the use of the: before, and
 * open_after functions.
 *
 *  \warning
 *  WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 *  While it provides some protection against eavesdropping, it does NOT
 *  provide any security against active attacks.
 *  Unless you know what you're doing, what you are looking for is probably
 *  the crypto_box functions.
 *
 * @{
 */

#include <stddef.h>

#include "crypto_stream_xsalsa20.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_stream_KEYBYTES crypto_stream_xsalsa20_KEYBYTES
  /// Size of Keys used
SODIUM_EXPORT
size_t  crypto_stream_keybytes(void);

#define crypto_stream_NONCEBYTES crypto_stream_xsalsa20_NONCEBYTES
  /// Size of nonces
SODIUM_EXPORT
size_t  crypto_stream_noncebytes(void);

#define crypto_stream_PRIMITIVE "xsalsa20"
  /// The underlying primitive.
SODIUM_EXPORT
const char *crypto_stream_primitive(void);

/**
 * Generates a stream using the given secret key and nonce.
 *
 * @param[out] stream the generated stream.
 * @param[out] slen   the length of the generated stream.
 * @param[in]  nonce  the nonce used to generate the stream.
 * @param[in]  key    the key used to generate the stream.
 *
 * @return 0 if operation successful
 *
 * @pre  nonce must have minimum length crypto_stream_NONCEBYTES
 * @pre  key must have minimum length crypto_stream_KEYBYTES
 * @post stream will have length slen
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_stream_KEYBYTES];
 * const unsigned char n[crypto_stream_NONCEBYTES];
 * unsigned char c[...]; unsigned long long clen;
 *
 * crypto_stream(c,clen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_stream(unsigned char *c, unsigned long long clen,
                  const unsigned char *n, const unsigned char *k);

/**
 * Encrypts the given message using the given secret key and nonce.
 *
 * The crypto_stream_xor function guarantees that the ciphertext is
 * the plaintext (xor) the output of crypto_stream. Consequently
 * crypto_stream_xor can also be used to decrypt.
 *
 * @param[out] ctxt  buffer for the resulting ciphertext.
 * @param[in]  msg   the message to be encrypted.
 * @param[in]  mlen  the length of the message.
 * @param[in]  nonce the nonce used during encryption.
 * @param[in]  key   secret key used during encryption.
 *
 * @return 0 if operation successful.
 *
 * @pre  ctxt must have length minimum mlen.
 * @pre  nonce must have length minimum crypto_stream_NONCEBYTES.
 * @pre  key must have length minimum crpyto_stream_KEYBYTES
 * @post first mlen bytes of ctxt will contain the ciphertext.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_stream_KEYBYTES];
 * const unsigned char n[crypto_stream_NONCEBYTES];
 * unsigned char m[...];
 * unsigned long long mlen;
 * unsigned char c[...];
 *
 * crypto_stream_xor(c,m,mlen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_stream_xor(unsigned char *c, const unsigned char *m,
                      unsigned long long mlen, const unsigned char *n,
                      const unsigned char *k);

  /** @} */
#ifdef __cplusplus
}
#endif

#endif
