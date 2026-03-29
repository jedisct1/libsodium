/*
 * src/ref/serpent_ctr.h
 *
 * Serpent-256 CTR mode streaming cipher — public API.
 *
 * Counter: 128-bit little-endian.  The nonce IS the initial counter value.
 * CTR is symmetric: the same function encrypts and decrypts.
 *
 * Key schedule must be computed with serpent_keyschedule() (serpent.h) before
 * calling any function here.
 */

#ifndef SERPENT_CTR_H
#define SERPENT_CTR_H

#include <stdint.h>
#include <stddef.h>

/*
 * serpent_ctr_xor — process a message with counter initialised to nonce.
 *
 *   sk    : 132-word subkey array from serpent_keyschedule()
 *   nonce : 16-byte counter start value (128-bit LE)
 *   in    : input  (plaintext or ciphertext)
 *   out   : output (may alias in)
 *   len   : byte count
 */
void serpent_ctr_xor(const uint32_t *sk,
                     const uint8_t   nonce[16],
                     const uint8_t  *in,
                     uint8_t        *out,
                     size_t          len);

/*
 * serpent_ctr_xor_ic — process with a 64-bit initial block-count offset.
 *
 *   ic    : block offset added to nonce (low 64 bits); seek to byte ic*16.
 */
void serpent_ctr_xor_ic(const uint32_t *sk,
                        const uint8_t   nonce[16],
                        uint64_t        ic,
                        const uint8_t  *in,
                        uint8_t        *out,
                        size_t          len);

/*
 * serpent_ctr_xor_ic128 — process with a full 128-bit block-count offset.
 *
 *   ic_lo : low  64 bits of the block offset
 *   ic_hi : high 64 bits of the block offset
 *
 * For worker-pool parallelism: each worker calls with its own (ic_lo, ic_hi)
 * start block and processes its chunk without sharing counter state.
 */
void serpent_ctr_xor_ic128(const uint32_t *sk,
                           const uint8_t   nonce[16],
                           uint64_t        ic_lo,
                           uint64_t        ic_hi,
                           const uint8_t  *in,
                           uint8_t        *out,
                           size_t          len);

#endif /* SERPENT_CTR_H */
