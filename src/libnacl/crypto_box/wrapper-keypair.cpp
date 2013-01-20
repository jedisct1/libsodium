#include <string>
using std::string;
#include "crypto_box.h"

string crypto_box_keypair(string *sk_string)
{
  unsigned char pk[crypto_box_PUBLICKEYBYTES];
  unsigned char sk[crypto_box_SECRETKEYBYTES];
  crypto_box_keypair(pk,sk);
  *sk_string = string((char *) sk,sizeof sk);
  return string((char *) pk,sizeof pk);
}
