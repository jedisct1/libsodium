#ifndef sign_ed25519_ref10_H
#define sign_ed25519_ref10_H

void _crypto_sign_ed25519_ref10_hinit(crypto_hash_sha512_state *hs,
                                      int prehashed);

int _crypto_sign_ed25519_detached(unsigned char *sig,
                                  sodium_size_t *siglen_p,
                                  const unsigned char *m,
                                  sodium_size_t mlen,
                                  const unsigned char *sk, int prehashed);

int _crypto_sign_ed25519_verify_detached(const unsigned char *sig,
                                         const unsigned char *m,
                                         sodium_size_t        mlen,
                                         const unsigned char *pk,
                                         int prehashed);
#endif
