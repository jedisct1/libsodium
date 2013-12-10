#ifndef crypto_sign_H
#define crypto_sign_H

/** \defgroup dsig Digital Signatures
 *
 * Definitions and functions to perform digital signatures.
 *
 * \warning
 * THREAD SAFETY: crypto_sign_keypair() is thread-safe,
 * provided that you called sodium_init() once before using any
 * other libsodium function.
 * Other functions, including crypto_sign_seed_keypair() are always thread-safe.
 *
 * @{
 */

#include <stddef.h>

#include "crypto_sign_ed25519.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_sign_BYTES crypto_sign_ed25519_BYTES
/// length of resulting signature.
SODIUM_EXPORT
size_t  crypto_sign_bytes(void);

#define crypto_sign_SEEDBYTES crypto_sign_ed25519_SEEDBYTES
/// @todo Document crypto_sign_SEEDBYTES
SODIUM_EXPORT
size_t  crypto_sign_seedbytes(void);

#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
/// length of verification key.

SODIUM_EXPORT
size_t  crypto_sign_publickeybytes(void);

#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
/// length of signing key.
SODIUM_EXPORT
size_t  crypto_sign_secretkeybytes(void);

#define crypto_sign_PRIMITIVE "ed25519"
/// The underlying primitive
SODIUM_EXPORT
const char *crypto_sign_primitive(void);

  /**
   *
   */
SODIUM_EXPORT
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed);

/**
 * Generates a signing/verification key pair.
 *
 * @param[out] vk the verification key.
 * @param[out] sk the signing key.
 *
 * @return 0 if operation successful.
 *
 * @pre the buffer for vk must be at least crypto_sign_PUBLICKEYBYTES in length
 * @pre the buffer for sk must be at least crypto_sign_SECRETKEYTBYTES in length
 * @post first crypto_sign_PUBLICKEYTBYTES of vk will be the key data.
 * @post first crypto_sign_SECRETKEYTBYTES of sk will be the key data.
 *
 *
 *~~~~~{.c}
 * unsigned char vk[crypto_sign_PUBLICKEYBYTES];
 * unsigned char sk[crypto_sign_SECRETKEYBYTES];
 *
 * crypto_sign_keypair(vk,sk);
 *~~~~~
 */

SODIUM_EXPORT
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

/**
 * Signs a given message using the signer's signing key.
 *
 * @param[out] sig   the resulting signature.
 * @param[out] slen  the length of the signature.
 * @param[in]  msg   the message to be signed.
 * @param[in]  mlen  the length of the message.
 * @param[in]  sk    the signing key.
 *
 * @return 0 if operation successful
 *
 * @pre sig must be of length mlen+crypto_sign_BYTES
 *
 *~~~~~{.c}
 * const unsigned char sk[crypto_sign_SECRETKEYBYTES];
 * const unsigned char m[...];
 * unsigned long long mlen;
 * unsigned char sm[...];
 * unsigned long long smlen;
 *
 * crypto_sign(sm,&smlen,m,mlen,sk);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

/**
 * Verifies the signed message sig using the signer's verification key.
 *
 * @param[out] msg the resulting message.
 * @param[out] mlen the length of msg.
 * @param[in]  sig the signed message.
 * @param[in]  smlen length of the signed message.
 * @param[in]  vk the verification key.
 *
 * @return 0 if successful, -1 if verification fails.
 *
 * @pre length of msg must be at least smlen
 *
 * @warning if verification fails msg may contain data from the
 * computation.
 *
 * Example innvocation:
 *
 *~~~~~{.c}
 * const unsigned char pk[crypto_sign_PUBLICKEYBYTES];
 * const unsigned char sm[...]; unsigned long long smlen;
 * unsigned char m[...]; unsigned long long mlen;
 *
 * crypto_sign_open(m,&mlen,sm,smlen,pk);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

  /** @} */
#ifdef __cplusplus
}
#endif

#endif
