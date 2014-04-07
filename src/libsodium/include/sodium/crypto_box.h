#ifndef crypto_box_H
#define crypto_box_H

/** \defgroup apke Authenticated Public-Key Encryption
 *
 * Definitions and functions to perform Authenticated Encryption.
 *
 * Authentication encryption provides guarantees towards the:
 *
 * - confidentiality
 * - integrity
 * - authenticity
 *
 * of data.
 *
 * Alongside the standard interface there also exists a
 * pre-computation interface. In the event that applications are
 * required to send several messages to the same receiver, speed can
 * be gained by splitting the operation into two steps: before and
 * after. Similarly applications that receive several messages from
 * the same sender can gain speed through the use of the: before, and
 * open_after functions.
 *
 *
 * \warning
 * THREAD SAFETY: crypto_box_keypair() is thread-safe,
 * provided that you called sodium_init() once before using any
 * other libsodium function.
 * Other functions are always thread-safe.
 *
 * @{
*/

#include <stddef.h>

#include "crypto_box_curve25519xsalsa20poly1305.h"
#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif


#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
/// Size of Public Key.
SODIUM_EXPORT
size_t  crypto_box_publickeybytes(void);

#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
/// Size of Secret Key
SODIUM_EXPORT
size_t  crypto_box_secretkeybytes(void);

#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
/// Size of the pre-computed ciphertext
SODIUM_EXPORT
size_t  crypto_box_beforenmbytes(void);

#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
/// Size of the Nonce
SODIUM_EXPORT
size_t  crypto_box_noncebytes(void);

#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
/// No. of leading 0 bytes in the message.
SODIUM_EXPORT
size_t  crypto_box_zerobytes(void);

#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
/// No. of leading 0 bytes in the cipher-text.
SODIUM_EXPORT
size_t  crypto_box_boxzerobytes(void);

#define crypto_box_MACBYTES crypto_box_curve25519xsalsa20poly1305_MACBYTES
///
SODIUM_EXPORT
size_t  crypto_box_macbytes(void);

#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"
/// The underlying cryptographic primitive.
SODIUM_EXPORT
const char *crypto_box_primitive(void);

/**
 * Randomly generates a secret key and a corresponding public key.
 *
 * @param[out] pk the buffer for the public key
 * @param[out] sk the buffer for the private key with length crypto_box_SECRETKEYTBYTES
 *
 * @return 0 if generation successful.
 *
 * @pre the buffer for pk must be at least crypto_box_PUBLICKEYBYTES in length
 * @pre the buffer for sk must be at least crypto_box_SECRETKEYTBYTES in length
 * @post first crypto_box_PUBLICKEYTBYTES of pk will be the key data.
 * @post first crypto_box_SECRETKEYTBYTES of sk will be the key data.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 *
 * unsigned char pk[crypto_box_PUBLICKEYBYTES];
 * unsigned char sk[crypto_box_SECRETKEYBYTES];
 *
 * crypto_box_keypair(pk,sk);
 *~~~~~
 *
 */
SODIUM_EXPORT
int crypto_box_keypair(unsigned char *pk, unsigned char *sk);

/**
 * Partially performs the computation required for both encryption and
 * decryption of data.
 *
 * @param[out] k  the result of the computation.
 * @param[in]  pk the receivers public key, used for encryption.
 * @param[in]  sk the senders private key, used for signing.
 *
 * The intermediate data computed by crypto_box_beforenm is suitable
 * for both crypto_box_afternm and crypto_box_open_afternm, and can be
 * reused for any number of messages.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * unsigned char k[crypto_box_BEFORENMBYTES];
 * const unsigned char pk[crypto_box_PUBLICKEYBYTES];
 * const unsigned char sk[crypto_box_SECRETKEYBYTES];
 *
 * crypto_box_beforenm(k,pk,sk);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_box_beforenm(unsigned char *k,
                        const unsigned char *pk,
                        const unsigned char *sk);

/**
 * Encrypts a given a message m, using partial computed data.
 *
 * @param[out] ctxt   the buffer for the cipher-text.
 * @param[in]  msg    the message to be encrypted.
 * @param[in]  mlen   the length of msg.
 * @param[in]  nonce  a randomly generated nonce.
 * @param[in]  k      the partial computed data.
 *
 * @return 0 if operation is successful.
 *
 * @pre  first crypto_box_ZEROBYTES of msg be all 0.
 * @pre  the nonce must have size crypto_box_NONCEBYTES.
 * @post first crypto_box_BOXZEROBYTES of ctxt be all 0.
 * @post first mlen bytes of ctxt will contain the ciphertext.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_box_BEFORENMBYTES];
 * const unsigned char n[crypto_box_NONCEBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * unsigned char c[...];
 *
 * crypto_box_afternm(c,m,mlen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_box_afternm(unsigned char *c,
                       const unsigned char *m,
                       unsigned long long mlen,
                       const unsigned char *n,
                       const unsigned char *k);

/**
 * Decrypts a ciphertext ctxt given the receivers private key, and
 * senders public key.
 *
 * @param[out] msg    the buffer to place resulting plaintext.
 * @param[in]  ctxt   the ciphertext to be decrypted.
 * @param[in]  clen   the length of the ciphertext.
 * @param[in]  nonce  a randomly generated nonce.
 * @param[in]  k      the partial computed data.
 *
 * @return 0 if successful and -1 if verification fails.
 *
 * @pre  first crypto_box_BOXZEROBYTES of ctxt be all 0.
 * @pre  the nonce must have size crypto_box_NONCEBYTES.
 * @post first clen bytes of msg will contain the plaintext.
 * @post first crypto_box_ZEROBYTES of msg will be all 0.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 *
 * const unsigned char k[crypto_box_BEFORENMBYTES];
 * const unsigned char n[crypto_box_NONCEBYTES];
 * const unsigned char c[...]; unsigned long long clen;
 * unsigned char m[...];
 *
 * crypto_box_open_afternm(m,c,clen,n,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_box_open_afternm(unsigned char *m,
                            const unsigned char *c,
                            unsigned long long clen,
                            const unsigned char *n,
                            const unsigned char *k);

/**
 * Encrypts a message given the senders secret key, and receivers
 * public key.
 *
 * @param[out] ctxt   the buffer for the cipher-text.
 * @param[in]  msg    the message to be encrypted.
 * @param[in]  mlen   the length of msg.
 * @param[in]  nonce  a randomly generated nonce.
 * @param[in]  pk     the receivers public key, used for encryption.
 * @param[in]  sk     the senders private key, used for signing.
 *
 * @return 0 if operation is successful.
 *
 * @pre  first crypto_box_ZEROBYTES of msg be all 0.
 * @pre  the nonce must have size crypto_box_NONCEBYTES.
 * @post first crypto_box_BOXZEROBYTES of ctxt be all 0.
 * @post first mlen bytes of ctxt will contain the ciphertext.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * const unsigned char pk[crypto_box_PUBLICKEYBYTES];
 * const unsigned char sk[crypto_box_SECRETKEYBYTES];
 * const unsigned char n[crypto_box_NONCEBYTES];
 * const unsigned char m[...];
 * unsigned long long mlen;
 * unsigned char c[...];
 *
 * crypto_box(c,m,mlen,n,pk,sk);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_box(unsigned char *c,
               const unsigned char *m,
               unsigned long long mlen,
               const unsigned char *n,
               const unsigned char *pk,
               const unsigned char *sk);

/**
 * Decrypts a ciphertext ctxt given the receivers private key, and
 * senders public key.
 *
 * @param[out] msg    the buffer to place resulting plaintext.
 * @param[in]  ctxt   the ciphertext to be decrypted.
 * @param[in]  clen   the length of the ciphertext.
 * @param[in]  nonce  a randomly generated.
 * @param[in]  pk     the senders public key, used for verification.
 * @param[in]  sk     the receivers private key, used for decryption.
 *
 * @return 0 if successful and -1 if verification fails.
 *
 * @pre  first crypto_box_BOXZEROBYTES of ctxt be all 0.
 * @pre  the nonce must have size crypto_box_NONCEBYTES.
 * @post first clen bytes of msg will contain the plaintext.
 * @post first crypto_box_ZEROBYTES of msg will be all 0.
 *
 * Example invocation:
 *
 *~~~~~{.c}
 * const unsigned char pk[crypto_box_PUBLICKEYBYTES];
 * const unsigned char sk[crypto_box_SECRETKEYBYTES];
 * const unsigned char n[crypto_box_NONCEBYTES];
 * const unsigned char c[...]; unsigned long long clen;
 * unsigned char m[...];
 *
 * crypto_box_open(m,c,clen,n,pk,sk);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_box_open(unsigned char *m,
                    const unsigned char *c,
                    unsigned long long clen,
                    const unsigned char *n,
                    const unsigned char *pk,
                    const unsigned char *sk);

SODIUM_EXPORT
int crypto_box_easy(unsigned char *c, const unsigned char *m,
                    unsigned long long mlen, const unsigned char *n,
                    const unsigned char *pk, const unsigned char *sk);

SODIUM_EXPORT
int crypto_box_open_easy(unsigned char *m, const unsigned char *c,
                         unsigned long long clen, const unsigned char *n,
                         const unsigned char *pk, const unsigned char *sk);

  /** @} */
#ifdef __cplusplus
}
#endif

#endif
