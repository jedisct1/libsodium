#ifndef sign_mldsa44_ref_H
#define sign_mldsa44_ref_H

#include <stddef.h>
#include <stdint.h>

#include "private/quirks.h"

int mldsa44_ref_keypair(unsigned char *pk, unsigned char *sk);

int mldsa44_ref_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed);

int mldsa44_ref_sign_detached(unsigned char *sig, unsigned long long *siglen_p,
                              const unsigned char *m, unsigned long long mlen,
                              const unsigned char *ctx, size_t ctxlen, const unsigned char *sk);

int mldsa44_ref_verify_detached(const unsigned char *sig, const unsigned char *m,
                                unsigned long long mlen, const unsigned char *ctx, size_t ctxlen,
                                const unsigned char *pk);

#endif
