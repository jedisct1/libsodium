#ifndef sign_ed25519_ref10_H
#define sign_ed25519_ref10_H

#include "private/quirks.h"

void _crypto_sign_ed25519_ref10_hinit(crypto_hash_sha512_state *hs,
                                      int prehashed,
                                      const unsigned char *c,
                                      unsigned long long clen);

int _crypto_sign_ed25519_detached(unsigned char *sig,
                                  unsigned long long *siglen_p,
                                  const unsigned char *m,
                                  unsigned long long mlen,
                                  const unsigned char *sk,
                                  int prehashed,
                                  const unsigned char *c,
                                  unsigned long long clen);

int _crypto_sign_ed25519_verify_detached(const unsigned char *sig,
                                         const unsigned char *m,
                                         unsigned long long   mlen,
                                         const unsigned char *pk,
                                         int prehashed,
                                         const unsigned char *c,
                                         unsigned long long clen);
#endif
