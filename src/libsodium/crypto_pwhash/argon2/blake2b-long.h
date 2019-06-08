#ifndef blake2b_long_H
#define blake2b_long_H

#include <stddef.h>

#define blake2b_long crypto_pwhash_argon2__blake2b_long

int blake2b_long(void *pout, size_t outlen, const void *in, size_t inlen);

#endif
