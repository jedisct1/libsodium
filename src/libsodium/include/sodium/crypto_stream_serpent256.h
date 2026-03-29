/*
 * src/crypto_stream_serpent256.h
 *
 * crypto_stream_serpent256 — public API header.
 *
 * Serpent-256 in 128-bit little-endian CTR mode.
 *
 * WARNING: This is a stream cipher only.  It provides confidentiality
 * against passive eavesdroppers but NO authentication and NO integrity
 * protection.  An active attacker can flip ciphertext bits undetected.
 * Unless you know exactly what you need, use crypto_secretstream or
 * crypto_aead instead.
 *
 * Key:   256-bit (32 bytes).  Generate with crypto_stream_serpent256_keygen().
 * Nonce: 128-bit (16 bytes).  The nonce IS the initial counter value.
 *        Counter format: 128-bit little-endian; byte 0 is the LSB.
 *        A random nonce is safe for a single key; do not reuse (key, nonce)
 *        pairs.  The 128-bit counter space covers 2^128 blocks = 2^132 bytes;
 *        counter exhaustion is not a practical concern.
 *
 * Block size: 16 bytes (128-bit Serpent block = one CTR counter block).
 *
 * Follows the libsodium crypto_stream_chacha20 API shape.
 */

#ifndef crypto_stream_serpent256_H
#define crypto_stream_serpent256_H

#include <stddef.h>
#include <stdint.h>
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

/* ── Constants ───────────────────────────────────────────────────────────── */

#define crypto_stream_serpent256_KEYBYTES   32U
SODIUM_EXPORT
size_t crypto_stream_serpent256_keybytes(void);

#define crypto_stream_serpent256_NONCEBYTES 16U
SODIUM_EXPORT
size_t crypto_stream_serpent256_noncebytes(void);

#define crypto_stream_serpent256_MESSAGEBYTES_MAX SODIUM_SIZE_MAX
SODIUM_EXPORT
size_t crypto_stream_serpent256_messagebytes_max(void);

/* ── API ─────────────────────────────────────────────────────────────────── */

/*
 * crypto_stream_serpent256 — generate clen bytes of keystream into c.
 *
 * Equivalent to encrypting clen zero bytes.
 */
SODIUM_EXPORT
int crypto_stream_serpent256(unsigned char *c, unsigned long long clen,
                             const unsigned char *n, const unsigned char *k)
            __attribute__ ((nonnull));

/*
 * crypto_stream_serpent256_xor — encrypt or decrypt mlen bytes.
 *
 * CTR is symmetric: the same call encrypts and decrypts.
 * c and m may alias (in-place is safe).
 */
SODIUM_EXPORT
int crypto_stream_serpent256_xor(unsigned char *c, const unsigned char *m,
                                 unsigned long long mlen,
                                 const unsigned char *n,
                                 const unsigned char *k)
            __attribute__ ((nonnull(1, 4, 5)));

/*
 * crypto_stream_serpent256_xor_ic — encrypt/decrypt with a block-count offset.
 *
 * ic: 64-bit block count added to n before starting.
 *     Seek to byte (ic * 16) within the keystream.
 *
 * Worker-pool parallel use:
 *   chunk_blocks = chunk_bytes / 16  (round up for partial final block)
 *   for each worker w:
 *     xor_ic(out_w, in_w, len_w, n, w * chunk_blocks, k)
 *
 * The 64-bit ic covers 2^64 × 16 = 2^68 bytes per nonce — well beyond any
 * single-key usage limit in practice.  For the rare case where ic might
 * overflow 64 bits, use xor_ic128 below.
 */
SODIUM_EXPORT
int crypto_stream_serpent256_xor_ic(unsigned char *c, const unsigned char *m,
                                    unsigned long long mlen,
                                    const unsigned char *n, uint64_t ic,
                                    const unsigned char *k)
            __attribute__ ((nonnull(1, 4, 6)));

/*
 * crypto_stream_serpent256_xor_ic128 — encrypt/decrypt with a full 128-bit
 * block-count offset.
 *
 * ic_lo: low  64 bits of the block offset (counter bytes 0–7, LE)
 * ic_hi: high 64 bits of the block offset (counter bytes 8–15, LE)
 *
 * Power-user interface: full 128-bit counter positioning, bypasses the
 * dispatch vtable.  Same as crypto_stream_chacha20_ietf_ext_xor_ic in
 * spirit — a standalone function outside the implementation struct.
 *
 * For offsets that fit in 64 bits (the common case), prefer xor_ic — it
 * dispatches through the vtable and picks up the SIMD path when available.
 */
SODIUM_EXPORT
int crypto_stream_serpent256_xor_ic128(unsigned char *c, const unsigned char *m,
                                       unsigned long long mlen,
                                       const unsigned char *n,
                                       uint64_t ic_lo, uint64_t ic_hi,
                                       const unsigned char *k)
            __attribute__ ((nonnull(1, 4, 7)));

/*
 * crypto_stream_serpent256_keygen — fill k with crypto_stream_serpent256_KEYBYTES
 * random bytes using the platform RNG.
 */
SODIUM_EXPORT
void crypto_stream_serpent256_keygen(
    unsigned char k[crypto_stream_serpent256_KEYBYTES])
            __attribute__ ((nonnull));

/* ── Internal ────────────────────────────────────────────────────────────── */

int _crypto_stream_serpent256_pick_best_implementation(void);

#ifdef __cplusplus
}
#endif

#endif /* crypto_stream_serpent256_H */
