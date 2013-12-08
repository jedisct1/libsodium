#ifndef crypto_hash_H
#define crypto_hash_H

#include "crypto_hash_sha512.h"
#include "export.h"

/** 
 * \defgroup hash Methods for Hashing
 *
 * Utility function to allow for hash computation.
 *
 * @{
 */

#define crypto_hash_BYTES crypto_hash_sha512_BYTES            ///< @todo
#define crypto_hash_BLOCKBYTES crypto_hash_sha512_BLOCKBYTES  ///< @todo
#define crypto_hash_PRIMITIVE "sha512"                        ///< @todo

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute a crypto_hash_BYTES hash of the given message.
 *
 * @param[out] hbuf a buffer to store the resulting hash.
 * @param[in]  msg  the message to be hashed.
 * @param[in]  mlen the length of the message to be hashed.
 *
 * @return 0 if successful operation.
 *
 * @pre  hbuf must have length minimum crypto_hash_BYTES.
 * @post first crypto_hash_BYTES. of hbuf will contain the hash.
 *
 * Example Innvocation:
 *
 *~~~~~{.c}
 * const unsigned char m[...]; unsigned long long mlen;
 * unsigned char h[crypto_hash_BYTES];
 *
 * crypto_hash(h,m,mlen);
 *~~~~~
 */
SODIUM_EXPORT
int crypto_hash(unsigned char *out, const unsigned char *in,
                unsigned long long inlen);

  /// @}
#ifdef __cplusplus
}
#endif

#endif
