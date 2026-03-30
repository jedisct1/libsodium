/*
 * src/wasm32/serpent_ctr_simd.c
 *
 * WASM SIMD Serpent-256 CTR mode: 4 counter blocks per SIMD iteration.
 *
 * Ported from leviathan-crypto/src/asm/serpent/ctr_simd.ts.
 * See serpent_simd.c for the block-cipher primitive (serpent_encrypt_4x).
 * See serpent_ctr.c  for the scalar CTR reference and counter conventions.
 *
 * Guarded by #if defined(__wasm_simd128__): produces no symbols elsewhere.
 *
 * Counter format: 128-bit little-endian, matching serpent_ctr.c.
 * Byte 0 is LSB; increment propagates carry from byte 0 upward.
 *
 * Interleaved block layout (matching serpent_simd.c serpent_encrypt_4x):
 *   W[w][lane] = word w of block lane,  w=0..3, lane=0..3.
 *
 * Word-from-counter byte order (matching serpent_encrypt load):
 *   word 0 = ctr[15]|(ctr[14]<<8)|(ctr[13]<<16)|(ctr[12]<<24)  <- MS word
 *   word 1 = ctr[11]|(ctr[10]<<8)|(ctr[ 9]<<16)|(ctr[ 8]<<24)
 *   word 2 = ctr[ 7]|(ctr[ 6]<<8)|(ctr[ 5]<<16)|(ctr[ 4]<<24)
 *   word 3 = ctr[ 3]|(ctr[ 2]<<8)|(ctr[ 1]<<16)|(ctr[ 0]<<24)  <- LS word
 *
 * Keystream-to-output byte order (matching serpent_encrypt store):
 *   ks[ 0.. 3] = word3 big-endian
 *   ks[ 4.. 7] = word2 big-endian
 *   ks[ 8..11] = word1 big-endian
 *   ks[12..15] = word0 big-endian
 */

#if defined(__wasm_simd128__)

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <wasm_simd128.h>

#include "../ref/serpent.h"        /* serpent_keyschedule, serpent_encrypt, serpent_wipe */
#include "../stream_serpent256.h"  /* crypto_stream_serpent256_implementation */
#include "utils.h"              /* sodium_memzero, SODIUM_C99 */

/* Forward declaration — defined in serpent_simd.c (same compilation unit). */
void serpent_encrypt_4x(const uint32_t *sk, v128_t *W);

/* ── Counter increment ───────────────────────────────────────────────────── */
/* Identical to ctr_inc() in serpent_ctr.c. Copied to avoid a cross-file
 * dependency in the wasm32 build path.                                     */

static inline void ctr_inc(uint8_t ctr[16])
{
	int i;
	for (i = 0; i < 16; i++)
		if (++ctr[i] != 0) break;
}

/* ── Counter word extraction ─────────────────────────────────────────────── */
/* Read one Serpent internal word from a counter block at the given byte
 * offset.  The 4-byte window starting at ctr[off] is loaded big-endian
 * (highest address = LSB), matching serpent_encrypt's load convention.     */

#define CTR_WORD(ctr, off) \
	((uint32_t)(ctr)[(off)+3]        | ((uint32_t)(ctr)[(off)+2] << 8) | \
	 ((uint32_t)(ctr)[(off)+1] << 16) | ((uint32_t)(ctr)[(off)+0] << 24))

/* ── Load 4 counter blocks into W[0..3] ─────────────────────────────────── */
/*
 * Reads 4 consecutive counter values (incrementing after each), converts
 * each to 4 Serpent internal words, and interleaves them:
 *   W[w] = i32x4(word_w_blk0, word_w_blk1, word_w_blk2, word_w_blk3)
 *
 * On return, ctr holds counter + 4 (ready for the next 4-block batch).
 * W[4] is scratch; caller must allocate but need not initialise it.
 */
static inline void load_counters_4x(uint8_t ctr[16], v128_t W[5])
{
	uint32_t b0w0, b0w1, b0w2, b0w3;
	uint32_t b1w0, b1w1, b1w2, b1w3;
	uint32_t b2w0, b2w1, b2w2, b2w3;
	uint32_t b3w0, b3w1, b3w2, b3w3;

	/* Block 0 — current counter. */
	b0w0 = CTR_WORD(ctr, 12); b0w1 = CTR_WORD(ctr,  8);
	b0w2 = CTR_WORD(ctr,  4); b0w3 = CTR_WORD(ctr,  0);
	ctr_inc(ctr);

	/* Block 1. */
	b1w0 = CTR_WORD(ctr, 12); b1w1 = CTR_WORD(ctr,  8);
	b1w2 = CTR_WORD(ctr,  4); b1w3 = CTR_WORD(ctr,  0);
	ctr_inc(ctr);

	/* Block 2. */
	b2w0 = CTR_WORD(ctr, 12); b2w1 = CTR_WORD(ctr,  8);
	b2w2 = CTR_WORD(ctr,  4); b2w3 = CTR_WORD(ctr,  0);
	ctr_inc(ctr);

	/* Block 3. */
	b3w0 = CTR_WORD(ctr, 12); b3w1 = CTR_WORD(ctr,  8);
	b3w2 = CTR_WORD(ctr,  4); b3w3 = CTR_WORD(ctr,  0);
	ctr_inc(ctr);

	/* Interleave: W[w][lane] = word_w of block_lane. */
	W[0] = wasm_i32x4_make((int32_t)b0w0, (int32_t)b1w0,
	                       (int32_t)b2w0, (int32_t)b3w0);
	W[1] = wasm_i32x4_make((int32_t)b0w1, (int32_t)b1w1,
	                       (int32_t)b2w1, (int32_t)b3w1);
	W[2] = wasm_i32x4_make((int32_t)b0w2, (int32_t)b1w2,
	                       (int32_t)b2w2, (int32_t)b3w2);
	W[3] = wasm_i32x4_make((int32_t)b0w3, (int32_t)b1w3,
	                       (int32_t)b2w3, (int32_t)b3w3);
}

/* ── Deinterleave one lane and XOR with input ────────────────────────────── */
/*
 * After serpent_encrypt_4x, W[0..3] still hold the interleaved output.
 * For lane L:
 *   keystream word w = W[w][L]  (i32x4 lane L)
 *   ks[ 0.. 3] = word3 BE,  ks[ 4.. 7] = word2 BE
 *   ks[ 8..11] = word1 BE,  ks[12..15] = word0 BE
 *
 * wasm_i32x4_extract_lane requires a literal constant for the lane argument.
 * We therefore expand one inline function per lane (0..3) via a macro.
 * This is the same pattern the TS compiler emits for i32x4.extract_lane.
 */

#define XOR_KS_LANE(LANE, in, out, r0, r1, r2, r3) do {            \
	uint32_t _w0 = (uint32_t)wasm_i32x4_extract_lane(r0, LANE); \
	uint32_t _w1 = (uint32_t)wasm_i32x4_extract_lane(r1, LANE); \
	uint32_t _w2 = (uint32_t)wasm_i32x4_extract_lane(r2, LANE); \
	uint32_t _w3 = (uint32_t)wasm_i32x4_extract_lane(r3, LANE); \
	(out)[ 0] = (in)[ 0] ^ (uint8_t)(_w3 >> 24);                \
	(out)[ 1] = (in)[ 1] ^ (uint8_t)(_w3 >> 16);                \
	(out)[ 2] = (in)[ 2] ^ (uint8_t)(_w3 >>  8);                \
	(out)[ 3] = (in)[ 3] ^ (uint8_t)(_w3      );                \
	(out)[ 4] = (in)[ 4] ^ (uint8_t)(_w2 >> 24);                \
	(out)[ 5] = (in)[ 5] ^ (uint8_t)(_w2 >> 16);                \
	(out)[ 6] = (in)[ 6] ^ (uint8_t)(_w2 >>  8);                \
	(out)[ 7] = (in)[ 7] ^ (uint8_t)(_w2      );                \
	(out)[ 8] = (in)[ 8] ^ (uint8_t)(_w1 >> 24);                \
	(out)[ 9] = (in)[ 9] ^ (uint8_t)(_w1 >> 16);                \
	(out)[10] = (in)[10] ^ (uint8_t)(_w1 >>  8);                \
	(out)[11] = (in)[11] ^ (uint8_t)(_w1      );                \
	(out)[12] = (in)[12] ^ (uint8_t)(_w0 >> 24);                \
	(out)[13] = (in)[13] ^ (uint8_t)(_w0 >> 16);                \
	(out)[14] = (in)[14] ^ (uint8_t)(_w0 >>  8);                \
	(out)[15] = (in)[15] ^ (uint8_t)(_w0      );                \
} while (0)

/* XOR all 4 lanes (64 bytes) with input. */
static inline void xor_keystream_4x(const uint8_t *in, uint8_t *out,
                                     v128_t r0, v128_t r1,
                                     v128_t r2, v128_t r3)
{
	XOR_KS_LANE(0, in,      out,      r0, r1, r2, r3);
	XOR_KS_LANE(1, in+16,   out+16,   r0, r1, r2, r3);
	XOR_KS_LANE(2, in+32,   out+32,   r0, r1, r2, r3);
	XOR_KS_LANE(3, in+48,   out+48,   r0, r1, r2, r3);
}

/* ── ctr_add64 ───────────────────────────────────────────────────────────── */
/*
 * Add a 64-bit block-count offset to a 128-bit LE counter in-place.
 * Carry propagates from the low 8 bytes into the high 8 bytes.
 * Identical to ctr_add64() in serpent_ctr.c — duplicated here to avoid
 * a cross-unit dependency in the wasm32 build.
 */
static inline void ctr_add64(uint8_t ctr[16], uint64_t n)
{
	int      i;
	uint32_t carry = 0;

	for (i = 0; i < 8; i++) {
		uint32_t s = (uint32_t)ctr[i] + (uint32_t)(n & 0xff) + carry;
		ctr[i] = (uint8_t)s;
		carry = s >> 8;
		n >>= 8;
	}
	for (i = 8; i < 16 && carry; i++) {
		uint32_t s = (uint32_t)ctr[i] + carry;
		ctr[i] = (uint8_t)s;
		carry = s >> 8;
	}
}


/* ── Scalar tail block ───────────────────────────────────────────────────── */
/*
 * Process a single counter block (1..16 bytes) using the scalar encrypt.
 * The counter is incremented before returning.
 */
static inline void process_block_scalar(const uint32_t *sk,
                                        uint8_t ctr[16],
                                        const uint8_t *in,
                                        uint8_t *out,
                                        size_t blen)
{
	uint8_t ks[16];
	size_t  i;
	serpent_encrypt(sk, ctr, ks);
	for (i = 0; i < blen; i++) out[i] = in[i] ^ ks[i];
	ctr_inc(ctr);
	sodium_memzero(ks, sizeof ks);
}

/* ── Core CTR stream ─────────────────────────────────────────────────────── */
/*
 * serpent_ctr_simd_core — inner loop for the wasm32 path.
 *
 * SIMD path:  processes 64 bytes (4 blocks) per iteration.
 * Scalar tail: processes remaining 0..3 blocks one at a time.
 *
 * ctr is mutated in-place; caller initialises it from nonce ± offset.
 */
static void
serpent_ctr_simd_core(const uint32_t *sk,
                      uint8_t ctr[16],
                      const uint8_t *in,
                      uint8_t *out,
                      size_t len)
{
	v128_t W[5];

	/* ── 4-wide SIMD path ── */
	while (len >= 64) {
		load_counters_4x(ctr, W);          /* load 4 counter blocks → W; ctr += 4 */
		serpent_encrypt_4x(sk, W);         /* encrypt in-place                     */
		xor_keystream_4x(in, out, W[0], W[1], W[2], W[3]);
		in  += 64;
		out += 64;
		len -= 64;
	}

	/* ── Scalar tail: 0..3 remaining blocks ── */
	while (len > 0) {
		size_t blen = len < 16 ? len : 16;
		process_block_scalar(sk, ctr, in, out, blen);
		in  += blen;
		out += blen;
		len -= blen;
	}

	sodium_memzero(W, sizeof W);
}

/* ── Entry points ────────────────────────────────────────────────────────── */

static int
stream_wasm32(unsigned char *c, unsigned long long clen,
              const unsigned char *n, const unsigned char *k)
{
	uint32_t sk[132];
	uint8_t  ctr[16];
	int      ret;

	if (clen <= 0ULL) return 0;
	if ((ret = serpent_keyschedule(k, 32, sk)) != 0) return ret;
	memset(c, 0, (size_t)clen);
	memcpy(ctr, n, 16);
	serpent_ctr_simd_core(sk, ctr, c, c, (size_t)clen);
	serpent_wipe(sk);
	sodium_memzero(ctr, sizeof ctr);
	return 0;
}

static int
stream_wasm32_xor_ic(unsigned char *c, const unsigned char *m,
                     unsigned long long mlen,
                     const unsigned char *n, uint64_t ic,
                     const unsigned char *k)
{
	uint32_t sk[132];
	uint8_t  ctr[16];
	int      ret;

	if (mlen <= 0ULL) return 0;
	if ((ret = serpent_keyschedule(k, 32, sk)) != 0) return ret;
	memcpy(ctr, n, 16);
	if (ic) ctr_add64(ctr, ic);
	serpent_ctr_simd_core(sk, ctr, m, c, (size_t)mlen);
	serpent_wipe(sk);
	sodium_memzero(ctr, sizeof ctr);
	return 0;
}


/* ── wasm32 vtable registration ──────────────────────────────────────────── */

struct crypto_stream_serpent256_implementation
    crypto_stream_serpent256_wasm32_implementation = {
        SODIUM_C99(.stream        =) stream_wasm32,
        SODIUM_C99(.stream_xor_ic =) stream_wasm32_xor_ic,
    };


#endif /* __wasm_simd128__ */
