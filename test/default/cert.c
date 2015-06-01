
#define TEST_NAME "cert"
#include "cmptest.h"

int
main(void)
{
  unsigned char pk[crypto_sign_PUBLICKEYBYTES];
  unsigned char sk[crypto_sign_SECRETKEYBYTES];
  unsigned char c[crypto_cert_BYTES];
  const char *ad = "Blue fish eat red food, but only on Sundays.";
  const char *ad_bad = "Blue fish eat red food, but only on Fridays.";
  crypto_sign_keypair(pk, sk);

  //create & verify
  if(crypto_cert(c, 0, 0, time(NULL) + 60, NULL, 0, pk, pk, sk))
    printf("crypto_cert() failed\n");
  if(crypto_cert_verify(c, NULL, 0))
    printf("crypto_cert_verify() failed\n");
  c[0] = 0;
  if(!crypto_cert_verify(c, NULL, 0))
    printf("Cert is invalid, but crypto_cert_verify() passed it anyway\n");

  //validity period
  assert(!crypto_cert(c, 0, time(NULL) + 60, time(NULL) + 120, NULL, 0, pk, pk, sk));
  if(!crypto_cert_verify(c, NULL, 0))
    printf("Cert is not yet valid, but crypto_cert_verify() passed it anyway\n");
  assert(!crypto_cert(c, 0, 0, time(NULL) - 120, NULL, 0, pk, pk, sk));
  if(!crypto_cert_verify(c, NULL, 0))
    printf("Cert has expired, but crypto_cert_verify() passed it anyway\n");

  //additional data
  assert(!crypto_cert(c, 0, 0, time(NULL) + 60, ad, strlen(ad) + 1, pk, pk, sk));
  if(crypto_cert_verify(c, ad, strlen(ad) + 1))
    printf("crypto_cert_verify() failed with additional data\n");
  if(!crypto_cert_verify(c, ad_bad, strlen(ad_bad) + 1))
    printf("Additional data is invalid, but crypto_cert_verify() passed it anyway\n");

  return 0;
}
