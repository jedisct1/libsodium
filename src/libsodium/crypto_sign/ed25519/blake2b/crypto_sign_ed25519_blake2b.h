#ifndef sign_ed25519_blake2b_H
#define sign_ed25519_blake2b_H

void _crypto_sign_ed25519_blake2b_hinit(crypto_generichash_blake2b_state *hs,
                                        const unsigned char *salt,
                                        const unsigned char *personal);

int _crypto_sign_ed25519_blake2b_detached(unsigned char *sig,
                                          unsigned long long *siglen_p,
                                          const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *sk,
                                          const unsigned char *personal);

int _crypto_sign_ed25519_blake2b_verify_detached(const unsigned char *sig,
                                                 const unsigned char *m,
                                                 unsigned long long   mlen,
                                                 const unsigned char *pk,
                                                 const unsigned char *personal);
#endif
