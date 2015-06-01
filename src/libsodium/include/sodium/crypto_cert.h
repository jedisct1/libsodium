#ifndef crypto_cert_H
#define crypto_cert_H

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "crypto_sign.h"
#include "crypto_generichash.h"
#include "export.h"

#define crypto_cert_VERSION 1

#define crypto_cert_BYTES (sizeof(uint8_t) /*version*/ \
                          +sizeof(uint8_t) /*flags*/ \
                          +sizeof(uint32_t) /*valid from*/ \
                          +sizeof(uint32_t) /*valid until*/ \
                          +crypto_sign_PUBLICKEYBYTES /*pk*/ \
                          +crypto_sign_PUBLICKEYBYTES /*signer*/ \
                          +crypto_sign_BYTES /*signature*/ \
                          )

SODIUM_EXPORT
int crypto_cert(unsigned char *out,
                uint8_t flags,
                time_t valid_from,
                time_t valid_until,
                const unsigned char *ad,
                unsigned long long adlen,
                const unsigned char *pk,
                const unsigned char *s,
                const unsigned char *k);

SODIUM_EXPORT
int crypto_cert_verify(const unsigned char *c, const unsigned char *ad,
                       unsigned long long adlen);

SODIUM_EXPORT
void crypto_cert_pk(unsigned char *out, const unsigned char *c);

SODIUM_EXPORT
void crypto_cert_signer(unsigned char *out, const unsigned char *c);

SODIUM_EXPORT
void crypto_cert_signature(unsigned char *out, const unsigned char *c);

SODIUM_EXPORT
time_t crypto_cert_valid_from(const unsigned char *c);

SODIUM_EXPORT
time_t crypto_cert_valid_until(const unsigned char *c);

SODIUM_EXPORT
uint8_t crypto_cert_version(const unsigned char *c);

SODIUM_EXPORT
uint8_t crypto_cert_flags(const unsigned char *c);

#endif
