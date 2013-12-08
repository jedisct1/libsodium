#ifndef crypto_onetimeauth_H
#define crypto_onetimeauth_H

#include <stddef.h>

#include "crypto_onetimeauth_poly1305.h"
#include "export.h"


/**
 * \defgroup onetimeauth One-Time Authentication
 *
 * Methods for one-time authentication.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_onetimeauth_BYTES crypto_onetimeauth_poly1305_BYTES
  /// Size of the authentication token.
SODIUM_EXPORT
size_t  crypto_onetimeauth_bytes(void);

#define crypto_onetimeauth_KEYBYTES crypto_onetimeauth_poly1305_KEYBYTES
  /// Size of the secret key used.
SODIUM_EXPORT
size_t  crypto_onetimeauth_keybytes(void);

#define crypto_onetimeauth_PRIMITIVE "poly1305"
  /// Return the underyling primitive.
SODIUM_EXPORT
const char *crypto_onetimeauth_primitive(void);

/**
 * Constructs a one time authentication token for the given message msg using a given secret key.
 *
 * @param[out] tok  the generated token.
 * @param[in]  msg  the message to be authenticated.
 * @param[in]  mlen the length of msg.
 * @param[in]  key  the key used to compute the authentication.
 *
 * @return 0 if operation successful.
 *
 * @pre  token must have minimum length crypto_onetimeauth_BYTES
 * @pre  key must be of length crypto_onetimeauth_KEY_BYTES
 * @post the first crypto_onetimeauth_BYTES of the token will contain the result.
 *
 * Example innvocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_onetimeauth_KEYBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * unsigned char a[crypto_onetimeauth_BYTES];

 * crypto_onetimeauth(a,m,mlen,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_onetimeauth(unsigned char *out, const unsigned char *in,
                       unsigned long long inlen, const unsigned char *k);

/**
 * Verifies that the given authentication token is correct for the
 * given message and key.
 *
 * @param[out] tok the generated token.
 * @param[in]  msg  the message to be authenticated.
 * @param[in]  mlen the length of msg.
 * @param[in]  key  the key used to compute the authentication.
 *
 * @return 0 if tok is the correct token for msg under the
 * given key. Otherwise -1.
 *
 * @pre  tok must have minimum length crypto_onetimeauth_BYTES
 * @pre  key must be of length crypto_onetimeauth_KEY_BYTES
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_onetimeauth_KEYBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * const unsigned char a[crypto_onetimeauth_BYTES];
 *
 * crypto_onetimeauth_verify(a,m,mlen,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_onetimeauth_verify(const unsigned char *h, const unsigned char *in,
                              unsigned long long inlen, const unsigned char *k);

  /// @}
#ifdef __cplusplus
}
#endif

#endif
