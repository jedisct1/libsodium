#ifndef crypto_hash_H
#define crypto_hash_H

#include "crypto_hash_sha256.h"

#define crypto_hash crypto_hash_sha256
#define crypto_hash_BYTES crypto_hash_sha256_BYTES
#define crypto_hash_PRIMITIVE "sha256"
#define crypto_hash_IMPLEMENTATION crypto_hash_sha256_IMPLEMENTATION
#define crypto_hash_VERSION crypto_hash_sha256_VERSION

#endif
