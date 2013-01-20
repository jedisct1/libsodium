#ifndef GE25519_H
#define GE25519_H

#include "fe25519.h"
#include "sc25519.h"

#define ge25519                           crypto_sign_ed25519_ref_ge25519
#define ge25519_base                      crypto_sign_ed25519_ref_ge25519_base
#define ge25519_unpackneg_vartime         crypto_sign_ed25519_ref_unpackneg_vartime
#define ge25519_pack                      crypto_sign_ed25519_ref_pack
#define ge25519_isneutral_vartime         crypto_sign_ed25519_ref_isneutral_vartime
#define ge25519_double_scalarmult_vartime crypto_sign_ed25519_ref_double_scalarmult_vartime
#define ge25519_scalarmult_base           crypto_sign_ed25519_ref_scalarmult_base

typedef struct
{
  fe25519 x;
  fe25519 y;
  fe25519 z;
  fe25519 t;
} ge25519;

const ge25519 ge25519_base;

int ge25519_unpackneg_vartime(ge25519 *r, const unsigned char p[32]);

void ge25519_pack(unsigned char r[32], const ge25519 *p);

int ge25519_isneutral_vartime(const ge25519 *p);

void ge25519_double_scalarmult_vartime(ge25519 *r, const ge25519 *p1, const sc25519 *s1, const ge25519 *p2, const sc25519 *s2);

void ge25519_scalarmult_base(ge25519 *r, const sc25519 *s);

#endif
