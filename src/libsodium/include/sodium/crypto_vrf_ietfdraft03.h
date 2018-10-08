
#ifndef crypto_vrf_ietfdraft03_H
#define crypto_vrf_ietfdraft03_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_vrf_ietfdraft03_PUBLICKEYBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft03_publickeybytes(void);

#define crypto_vrf_ietfdraft03_SECRETKEYBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft03_secretkeybytes(void);

#define crypto_vrf_ietfdraft03_SEEDBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft03_seedbytes(void);

#define crypto_vrf_ietfdraft03_PROOFBYTES 80U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft03_proofbytes(void);

#define crypto_vrf_ietfdraft03_OUTPUTBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft03_outputbytes(void);

// Generate a keypair.
//
// Thread-safe if sodium_init() has been called first.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_keypair(unsigned char *pk, unsigned char *sk);

// Generate a keypair from a seed.
//
// Convert a 32-byte seed into a keypair per RFC8032 section 5.1.5, except the
// secret key we return has the public key appended. In particular, we hash the
// seed with SHA512. The first 32 bytes, after clamping, are the secret scalar,
// and the public key is the base point times the secret scalar. The 64-byte
// augmented secret key we return is the 32-byte seed concatenated with the
// 32-byte public key.
//
// In the IETF draft spec, the secret key is a 32-byte string from which the
// secret scalar, the secret nonce prefix, and the public point are computed.
// To avoid having to recompute the public point from the secret key every time
// we call vrf_prove, the "secret key" our keygen function returns to the user
// will actually be the secret key with the public key appended. To avoid
// confusion, we'll use "seed" to refer to the 32-byte string called the "secret
// key" in the IETF spec, and "augmented secret key" to refer to the the 64-byte
// string returned by our keygen function. libsodium's crypto_sign_ed25519
// takes the same approach.
//
// Constant time.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_keypair_from_seed(unsigned char *pk,
					     unsigned char *sk,
					     const unsigned char *seed);

// Returns 1 if public key is valid (per IETF spec section 5.6.1); 0 if invalid.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_is_valid_key(const unsigned char *pk)
            __attribute__ ((warn_unused_result));

// Generate a VRF proof for a message using a secret key.
//
// The VRF output hash can be obtained by calling crypto_vrf_proof_to_hash(proof).
// 
// Returns 0 on success, -1 on error decoding the (augmented) secret key
//
// This runs in time constant with respect to sk and, fixing a value of mlen,
// runs in time constant with respect to m.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_prove(unsigned char *proof, const unsigned char *sk,
				 const unsigned char *m,
				 unsigned long long mlen);

// Verify a VRF proof (for a given a public key and message) and validate the
// public key.
//
// For a given public key and message, there are many possible proofs but only
// one possible output hash.
//
// Returns 0 if verification succeeds and -1 on failure. If the public key is
// valid and verification succeeds, the output hash is stored in output.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_verify(unsigned char *output,
				  const unsigned char *pk,
				  const unsigned char *proof,
				  const unsigned char *m,
				  unsigned long long mlen)
            __attribute__ ((warn_unused_result));

// Convert a VRF proof to a VRF output.
//
// This function does not verify the proof.
//
// Returns 0 on success, nonzero on error decoding.
SODIUM_EXPORT
int crypto_vrf_ietfdraft03_proof_to_hash(unsigned char *hash,
				         const unsigned char *proof);

// Convert a secret key to a public key.
//
// Constant time.
SODIUM_EXPORT
void crypto_vrf_ietfdraft03_sk_to_pk(unsigned char *pk,
				     const unsigned char *sk);

// Convert a secret key to the seed that generated it.
//
// Constant time.
SODIUM_EXPORT
void crypto_vrf_ietfdraft03_sk_to_seed(unsigned char *seed,
				       const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif
