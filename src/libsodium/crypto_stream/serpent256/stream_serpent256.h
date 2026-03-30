/*
 * src/stream_serpent256.h
 *
 * Internal vtable for crypto_stream_serpent256 implementations.
 *
 * Follows the libsodium stream cipher dispatch pattern established by
 * crypto_stream/chacha20/stream_chacha20.h.  Each implementation
 * (ref, wasm32) provides a file-scoped definition of the struct and
 * exposes it via the extern declarations below.
 *
 * Implementations:
 *   ref    — scalar Boolean-circuit Serpent (src/ref/serpent.c + serpent_ctr.c)
 *   wasm32 — WASM SIMD 4-wide CTR (src/wasm32/serpent_simd.c + serpent_ctr_simd.c)
 *
 * The wasm32 entry points are guarded by #if defined(__wasm_simd128__).
 * pick_best selects wasm32 at compile time when the guard fires; otherwise
 * falls through to ref, matching the argon2 fill-block-wasm32 precedent.
 */

#ifndef STREAM_SERPENT256_H
#define STREAM_SERPENT256_H

#include <stdint.h>

typedef struct crypto_stream_serpent256_implementation {
	/*
	 * stream — generate clen bytes of keystream into c.
	 * Equivalent to xor_ic(c, zeros, clen, n, 0, k).
	 */
	int (*stream)(unsigned char       *c,
	              unsigned long long   clen,
	              const unsigned char *n,
	              const unsigned char *k);

	/*
	 * stream_xor_ic — XOR message m with keystream, writing to c.
	 * ic is a block-count offset: counter starts at nonce + ic.
	 * c and m may alias (in-place is safe).
	 */
	int (*stream_xor_ic)(unsigned char       *c,
	                     const unsigned char *m,
	                     unsigned long long   mlen,
	                     const unsigned char *n,
	                     uint64_t             ic,
	                     const unsigned char *k);
} crypto_stream_serpent256_implementation;

/* ── Implementation declarations ────────────────────────────────────────── */

/* Scalar Boolean-circuit reference implementation (always available). */
extern struct crypto_stream_serpent256_implementation
    crypto_stream_serpent256_ref_implementation;

/* WASM SIMD 4-wide CTR implementation (requires __wasm_simd128__). */
#if defined(__wasm_simd128__)
extern struct crypto_stream_serpent256_implementation
    crypto_stream_serpent256_wasm32_implementation;
#endif

#endif /* STREAM_SERPENT256_H */
