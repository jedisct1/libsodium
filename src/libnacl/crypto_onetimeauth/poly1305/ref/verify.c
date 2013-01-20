#include "crypto_verify_16.h"
#include "crypto_onetimeauth.h"

int crypto_onetimeauth_verify(const unsigned char *h,const unsigned char *in,unsigned long long inlen,const unsigned char *k)
{
  unsigned char correct[16];
  crypto_onetimeauth(correct,in,inlen,k);
  return crypto_verify_16(h,correct);
}
