/*
 * src/ref/serpent.h
 *
 * Serpent-256 block cipher — scalar C reference implementation.
 *
 * Boolean-circuit S-boxes (constant-time: AND/OR/XOR/NOT only, no table
 * lookups, no data-dependent memory access).  Key schedule derived from the
 * Serpent AES submission (Anderson/Biham/Knudsen, 1998).  Byte ordering
 * matches leviathan-crypto and the ctr_harness.c CTR vector generation
 * harness; all three produce identical ECB output for the same key/plaintext.
 *
 * Verified against:
 *   - NESSIE test vectors (128/192/256-bit keys)
 *   - AES candidate evaluation ECB KAT and Monte Carlo vectors
 *   - leviathan-crypto CTR vectors (ctr_harness.c confirmed match)
 */

#ifndef SERPENT_H
#define SERPENT_H

#include <stdint.h>
#include <stddef.h>

/*
 * serpent_keyschedule — expand a key into 132 subkey words (33 × 4).
 *
 *   key    : raw key bytes, little-endian
 *   keylen : key length in bytes: 16 (128-bit), 24 (192-bit), or 32 (256-bit)
 *   sk     : output array of 132 uint32_t subkey words
 *
 * Returns 0 on success, -1 if keylen is not 16, 24, or 32.
 * Shorter keys are padded to 256 bits per the Serpent specification.
 */
int serpent_keyschedule(const uint8_t *key, int keylen, uint32_t sk[132]);

/*
 * serpent_encrypt — encrypt one 16-byte block (ECB).
 *
 *   sk  : subkey array from serpent_keyschedule()
 *   pt  : 16-byte plaintext input
 *   ct  : 16-byte ciphertext output  (may alias pt)
 */
void serpent_encrypt(const uint32_t *sk, const uint8_t pt[16], uint8_t ct[16]);

/*
 * serpent_decrypt — decrypt one 16-byte block (ECB).
 *
 *   sk  : subkey array from serpent_keyschedule()
 *   ct  : 16-byte ciphertext input
 *   pt  : 16-byte plaintext output  (may alias ct)
 */
void serpent_decrypt(const uint32_t *sk, const uint8_t ct[16], uint8_t pt[16]);

/*
 * serpent_wipe — zero all 132 subkey words.
 *
 * Call when done with a key to remove sensitive material from memory.
 * Uses sodium_memzero() internally so the compiler cannot optimise away
 * the wipe.
 */
void serpent_wipe(uint32_t sk[132]);

#endif /* SERPENT_H */
