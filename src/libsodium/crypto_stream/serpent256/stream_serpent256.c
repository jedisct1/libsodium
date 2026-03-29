/*
 * src/stream_serpent256.c
 *
 * crypto_stream_serpent256 — dispatch layer and public API.
 *
 * Follows crypto_stream/chacha20/stream_chacha20.c.  A static pointer
 * `implementation` is initialised to the ref impl and may be replaced by
 * _crypto_stream_serpent256_pick_best_implementation() — called once during
 * sodium_init() via implementations.h registration.
 *
 * pick_best uses a compile-time guard, matching the argon2 fill-block-wasm32
 * precedent: no sodium_runtime_has_* query is needed because WASM SIMD is a
 * compile-time target, not a CPU feature toggled at runtime.
 *
 *   ref   → always compiled; selected when __wasm_simd128__ is absent
 *   wasm32 → selected at compile time when __wasm_simd128__ is defined
 *
 * This file also contains the ref vtable registration (the struct definition
 * for crypto_stream_serpent256_ref_implementation).  When this code is
 * submitted to libsodium proper, the registration should move into a
 * dedicated src/ref/stream_serpent256_ref.c following the chacha20 layout.
 * For the standalone contrib repo, keeping it here avoids an extra file.
 */

#include "crypto_stream_serpent256.h"
#include "stream_serpent256.h"
#include "core.h"
#include "private/common.h"
#include "private/implementations.h"
#include "randombytes.h"
#include "utils.h"

#include "ref/serpent.h"
#include "ref/serpent_ctr.h"

#include <stdint.h>
#include <string.h>

/* ── Ref implementation entry points ────────────────────────────────────── */
/*
 * These are thin wrappers that run the key schedule on each call, matching
 * libsodium convention (no persistent key-expansion state in the public API).
 * The 528-byte subkey array lives on the stack and is wiped before returning.
 */

static int
stream_ref(unsigned char *c, unsigned long long clen,
           const unsigned char *n, const unsigned char *k)
{
	uint32_t sk[132];
	int      ret;

	if (clen <= 0ULL) return 0;
	if ((ret = serpent_keyschedule(k, 32, sk)) != 0) return ret;
	memset(c, 0, (size_t)clen);
	serpent_ctr_xor(sk, n, c, c, (size_t)clen);
	serpent_wipe(sk);
	return 0;
}

static int
stream_ref_xor_ic(unsigned char *c, const unsigned char *m,
                  unsigned long long mlen,
                  const unsigned char *n, uint64_t ic,
                  const unsigned char *k)
{
	uint32_t sk[132];
	int      ret;

	if (mlen <= 0ULL) return 0;
	if ((ret = serpent_keyschedule(k, 32, sk)) != 0) return ret;
	serpent_ctr_xor_ic(sk, n, ic, m, c, (size_t)mlen);
	serpent_wipe(sk);
	return 0;
}

/* ── Ref vtable registration ─────────────────────────────────────────────── */

struct crypto_stream_serpent256_implementation
    crypto_stream_serpent256_ref_implementation = {
        SODIUM_C99(.stream    =) stream_ref,
        SODIUM_C99(.stream_xor_ic =) stream_ref_xor_ic,
    };

/* ── Dispatch pointer ────────────────────────────────────────────────────── */

static const crypto_stream_serpent256_implementation *implementation =
    &crypto_stream_serpent256_ref_implementation;

/* ── Public API ──────────────────────────────────────────────────────────── */

size_t
crypto_stream_serpent256_keybytes(void)
{
	return crypto_stream_serpent256_KEYBYTES;
}

size_t
crypto_stream_serpent256_noncebytes(void)
{
	return crypto_stream_serpent256_NONCEBYTES;
}

size_t
crypto_stream_serpent256_messagebytes_max(void)
{
	return crypto_stream_serpent256_MESSAGEBYTES_MAX;
}

/*
 * crypto_stream_serpent256 — write clen bytes of keystream to c.
 *
 * Equivalent to XORing clen zero bytes.  Useful for generating key material.
 */
int
crypto_stream_serpent256(unsigned char *c, unsigned long long clen,
                         const unsigned char *n, const unsigned char *k)
{
	if (clen > crypto_stream_serpent256_MESSAGEBYTES_MAX)
		sodium_misuse();
	return implementation->stream(c, clen, n, k);
}

/*
 * crypto_stream_serpent256_xor — encrypt or decrypt a message.
 *
 * Counter starts at nonce.  CTR is symmetric; call for both directions.
 */
int
crypto_stream_serpent256_xor(unsigned char *c, const unsigned char *m,
                             unsigned long long mlen,
                             const unsigned char *n, const unsigned char *k)
{
	if (mlen > crypto_stream_serpent256_MESSAGEBYTES_MAX)
		sodium_misuse();
	return implementation->stream_xor_ic(c, m, mlen, n, 0ULL, k);
}

/*
 * crypto_stream_serpent256_xor_ic — encrypt/decrypt with a block-count offset.
 *
 * ic is a 64-bit block count added to the nonce before starting.
 * Seek to byte position (ic * 16) within the keystream.
 *
 * Worker-pool use: split a large message into N chunks of block_count blocks
 * each; call this function per chunk with ic = chunk_index * block_count.
 * Each worker operates independently with no shared counter state.
 */
int
crypto_stream_serpent256_xor_ic(unsigned char *c, const unsigned char *m,
                                unsigned long long mlen,
                                const unsigned char *n, uint64_t ic,
                                const unsigned char *k)
{
	if (mlen > crypto_stream_serpent256_MESSAGEBYTES_MAX)
		sodium_misuse();
	return implementation->stream_xor_ic(c, m, mlen, n, ic, k);
}

/*
 * crypto_stream_serpent256_keygen — generate a random 256-bit key.
 */
void
crypto_stream_serpent256_keygen(
    unsigned char k[crypto_stream_serpent256_KEYBYTES])
{
	randombytes_buf(k, crypto_stream_serpent256_KEYBYTES);
}

/*
 * crypto_stream_serpent256_xor_ic128 — encrypt/decrypt with a full 128-bit
 * block-count offset.
 *
 * Power-user interface for the rare case where a 64-bit ic overflows.
 * Bypasses the vtable — direct call, always uses the ref CTR path.
 * (SIMD dispatch of the 4-wide loop still goes through xor_ic.)
 *
 * Worker-pool use with ic128: split the message at arbitrary 128-bit block
 * offsets.  Each worker calls xor_ic128 with its own (ic_lo, ic_hi).
 *
 * For offsets that fit in 64 bits, prefer xor_ic — it dispatches through the
 * vtable and picks up the SIMD path when available.
 */
int
crypto_stream_serpent256_xor_ic128(unsigned char *c, const unsigned char *m,
                                    unsigned long long mlen,
                                    const unsigned char *n,
                                    uint64_t ic_lo, uint64_t ic_hi,
                                    const unsigned char *k)
{
	uint32_t sk[132];
	int      ret;

	if (mlen <= 0ULL) return 0;
	if (mlen > crypto_stream_serpent256_MESSAGEBYTES_MAX)
		sodium_misuse();
	if ((ret = serpent_keyschedule(k, 32, sk)) != 0) return ret;
	serpent_ctr_xor_ic128(sk, n, ic_lo, ic_hi, m, c, (size_t)mlen);
	serpent_wipe(sk);
	return 0;
}

/* ── Implementation selection ────────────────────────────────────────────── */
/*
 * _crypto_stream_serpent256_pick_best_implementation
 *
 * Called once by sodium_init() (via implementations.h registration).
 * Selects the fastest available implementation at compile time.
 *
 * WASM SIMD is a compile-time target: __wasm_simd128__ is set by the
 * toolchain when targeting wasm32 with -msimd128.  No runtime CPU query
 * is needed or possible, matching the argon2-fill-block-wasm32 precedent.
 *
 * Fallthrough: when none of the accelerated guards fire, the ref
 * implementation remains selected (it was the initialiser of `implementation`).
 */
int
_crypto_stream_serpent256_pick_best_implementation(void)
{
/* LCOV_EXCL_START */
#if defined(__wasm_simd128__)
	implementation = &crypto_stream_serpent256_wasm32_implementation;
	return 0;
#endif
	implementation = &crypto_stream_serpent256_ref_implementation;
	return 0;
/* LCOV_EXCL_STOP */
}
