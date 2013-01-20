#include <string>
using std::string;
#include "crypto_sign.h"

string crypto_sign_keypair(string *sk_string)
{
  unsigned char pk[crypto_sign_PUBLICKEYBYTES];
  unsigned char sk[crypto_sign_SECRETKEYBYTES];
  crypto_sign_keypair(pk,sk);
  *sk_string = string((char *) sk,sizeof sk);
  return string((char *) pk,sizeof pk);
}
