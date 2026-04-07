#ifndef kem_mlkem768_ref_H
#define kem_mlkem768_ref_H

#include <stddef.h>
#include <stdint.h>

#include "private/quirks.h"

int mlkem768_ref_keypair(unsigned char *pk, unsigned char *sk);

int mlkem768_ref_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed);

int mlkem768_ref_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int mlkem768_ref_enc_deterministic(unsigned char *ct, unsigned char *ss, const unsigned char *pk,
                                   const unsigned char *seed);

int mlkem768_ref_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#endif
