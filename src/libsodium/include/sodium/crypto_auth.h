#ifndef crypto_auth_H
#define crypto_auth_H

#include <stddef.h>

#include "crypto_auth_hmacsha512256.h"
#include "export.h"

/**
 * \defgroup auth Authentication
 *
 * Methods for authentication.
 *
 * @{
 */


#ifdef __cplusplus
extern "C" {
#endif

#define crypto_auth_BYTES crypto_auth_hmacsha512256_BYTES
  /// Lenght of the authenticator
SODIUM_EXPORT
size_t  crypto_auth_bytes(void);

#define crypto_auth_KEYBYTES crypto_auth_hmacsha512256_KEYBYTES
  /// Length of the key used for authentication.
SODIUM_EXPORT
size_t  crypto_auth_keybytes(void);

#define crypto_auth_PRIMITIVE "hmacsha512256"
  /// The underlying primitive used.
SODIUM_EXPORT
const char *crypto_auth_primitive(void);

/**
 * Constructs a one time authentication token for the given message msg using a given secret key.
 *
 * @param[out] tok  the generated authentication token.
 * @param[in]  msg  the message to be authenticated.
 * @param[in]  mlen the length of msg.
 * @param[in]  key  the key used to compute the token.
 *
 * @return 0 if operation successful.
 *
 * @pre  tok must have minimum length crypto_auth_BYTES
 * @pre  key must be of length crypto_auth_KEY_BYTES
 * @post the first crypto_auth_BYTES of auth will contain the result.
 *
 * Example innvocation:
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_auth_KEYBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * unsigned char a[crypto_auth_BYTES];
 *
 * crypto_auth(a,m,mlen,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_auth(unsigned char *out, const unsigned char *in,
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
 * @pre  tok must have minimum length crypto_auth_BYTES
 * @pre  key must be of length crypto_auth_KEY_BYTES
 *
 *~~~~~{.c}
 * const unsigned char k[crypto_auth_KEYBYTES];
 * const unsigned char m[...]; unsigned long long mlen;
 * const unsigned char a[crypto_auth_BYTES];
 *
 * crypto_auth_verify(a,m,mlen,k);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_auth_verify(const unsigned char *h, const unsigned char *in,
                       unsigned long long inlen, const unsigned char *k);
  /// @}
#ifdef __cplusplus
}
#endif

#endif
