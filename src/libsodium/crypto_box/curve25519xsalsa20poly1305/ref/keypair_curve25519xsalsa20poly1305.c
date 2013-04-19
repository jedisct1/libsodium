#include "crypto_scalarmult_curve25519.h"
#include "api.h"
#include "randombytes.h"

int crypto_box_keypair(
  unsigned char *pk,
  unsigned char *sk
)
{
  randombytes(sk,32);
  return crypto_scalarmult_curve25519_base(pk,sk);
}
