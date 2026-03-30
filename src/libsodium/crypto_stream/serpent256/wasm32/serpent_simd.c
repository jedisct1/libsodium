/*
 * src/wasm32/serpent_simd.c
 *
 * AUTO-GENERATED — do not edit by hand.
 * Regenerate: bun generate/generate_serpent_simd_c.ts > src/wasm32/serpent_simd.c
 *
 * WASM SIMD Serpent-256: 4-wide inter-block parallelism.
 * Each v128_t lane holds word w from a different block (CTR counters or
 * CBC ciphertext blocks). Same inter-block model as ChaCha20 CTR-4.
 *
 * S-box circuits: constant-time Boolean gates (AND/OR/XOR/NOT only).
 * No table lookups, no data-dependent branches, no memory indexed by secret data.
 *
 * Generated: 2026-03-28T23:56:52.535Z
 */

#if defined(__wasm_simd128__)

#include <stdint.h>
#include <wasm_simd128.h>

/* ── rotl helper ─────────────────────────────────────────────────────────── */
/* Rotate-left all 4 × i32 lanes by n bits. */
static inline v128_t rotl_v(v128_t v, int n) {
	return wasm_v128_or(wasm_i32x4_shl(v, n), wasm_u32x4_shr(v, 32 - n));
}

/* ── Forward S-boxes (v128) ─────────────────────────────────────────────── */
/* Each gate operates on W[xi] where xi is the slot-index parameter.        */
/* W[xi][lane] = word xi from block lane. No cross-lane operations.         */

static inline void sb0_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x3];
	W[x3] = wasm_v128_or(W[x3], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
	W[x4] = wasm_v128_not(W[x4]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x4] = wasm_v128_or(W[x4], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x1]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x1] = wasm_v128_not(W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
}

static inline void sb1_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x1];
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x3] = wasm_v128_not(W[x3]);
	W[x4] = wasm_v128_and(W[x4], W[x1]);
	W[x0] = wasm_v128_or(W[x0], W[x1]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x1] = wasm_v128_or(W[x1], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x0]);
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x1] = wasm_v128_or(W[x1], W[x0]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
}

static inline void sb2_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x3] = wasm_v128_not(W[x3]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x4] = W[x0];
	W[x0] = wasm_v128_and(W[x0], W[x2]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x1]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x1] = wasm_v128_or(W[x1], W[x2]);
}

static inline void sb3_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x1];
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x0]);
	W[x4] = wasm_v128_and(W[x4], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x3]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x0] = wasm_v128_or(W[x0], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x4]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x4] = wasm_v128_or(W[x4], W[x1]);
	W[x2] = wasm_v128_and(W[x2], W[x1]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
}

static inline void sb4_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x3];
	W[x3] = wasm_v128_and(W[x3], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x4]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x2] = wasm_v128_or(W[x2], W[x4]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x2] = wasm_v128_or(W[x2], W[x0]);
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x4]);
	W[x4] = wasm_v128_and(W[x4], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x3] = wasm_v128_or(W[x3], W[x1]);
	W[x1] = wasm_v128_not(W[x1]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
}

static inline void sb5_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x1];
	W[x1] = wasm_v128_or(W[x1], W[x0]);
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x3] = wasm_v128_not(W[x3]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x1] = wasm_v128_and(W[x1], W[x4]);
	W[x4] = wasm_v128_or(W[x4], W[x3]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x3]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x2] = wasm_v128_and(W[x2], W[x4]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
}

static inline void sb6_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x1];
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x3]);
	W[x1] = wasm_v128_or(W[x1], W[x3]);
	W[x4] = wasm_v128_not(W[x4]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x2] = wasm_v128_and(W[x2], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x1]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
}

static inline void sb7_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x1] = wasm_v128_not(W[x1]);
	W[x4] = W[x1];
	W[x0] = wasm_v128_not(W[x0]);
	W[x1] = wasm_v128_and(W[x1], W[x2]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x0] = wasm_v128_or(W[x0], W[x1]);
	W[x2] = wasm_v128_and(W[x2], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x4] = wasm_v128_or(W[x4], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
}

/* ── Inverse S-boxes (v128) ─────────────────────────────────────────────── */

static inline void si0_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x3];
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x3] = wasm_v128_or(W[x3], W[x1]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x1]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
}

static inline void si1_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x4] = W[x0];
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_not(W[x2]);
	W[x4] = wasm_v128_or(W[x4], W[x1]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x1]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x1] = wasm_v128_or(W[x1], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
	W[x0] = wasm_v128_or(W[x0], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
}

static inline void si2_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x4] = W[x3];
	W[x3] = wasm_v128_not(W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x1] = wasm_v128_or(W[x1], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x4]);
	W[x4] = wasm_v128_or(W[x4], W[x3]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
}

static inline void si3_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x2] = wasm_v128_xor(W[x2], W[x1]);
	W[x4] = W[x1];
	W[x1] = wasm_v128_and(W[x1], W[x2]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_or(W[x0], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x1]);
	W[x1] = wasm_v128_xor(W[x1], W[x2]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x1]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
}

static inline void si4_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x4] = W[x0];
	W[x0] = wasm_v128_and(W[x0], W[x1]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_or(W[x2], W[x3]);
	W[x4] = wasm_v128_not(W[x4]);
	W[x1] = wasm_v128_xor(W[x1], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_and(W[x2], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
	W[x0] = wasm_v128_or(W[x0], W[x4]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x0] = wasm_v128_xor(W[x0], W[x3]);
}

static inline void si5_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x1];
	W[x1] = wasm_v128_or(W[x1], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x0]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x2] = wasm_v128_or(W[x2], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x4] = wasm_v128_and(W[x4], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x1] = wasm_v128_xor(W[x1], W[x3]);
	W[x0] = wasm_v128_and(W[x0], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_xor(W[x2], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
}

static inline void si6_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x4] = W[x0];
	W[x0] = wasm_v128_and(W[x0], W[x3]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x2] = wasm_v128_or(W[x2], W[x4]);
	W[x2] = wasm_v128_xor(W[x2], W[x3]);
	W[x3] = wasm_v128_and(W[x3], W[x0]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x2] = wasm_v128_xor(W[x2], W[x0]);
}

static inline void si7_v(v128_t *W, int x0, int x1, int x2, int x3, int x4) {
	W[x4] = W[x3];
	W[x3] = wasm_v128_and(W[x3], W[x0]);
	W[x0] = wasm_v128_xor(W[x0], W[x2]);
	W[x2] = wasm_v128_or(W[x2], W[x4]);
	W[x4] = wasm_v128_xor(W[x4], W[x1]);
	W[x0] = wasm_v128_not(W[x0]);
	W[x1] = wasm_v128_or(W[x1], W[x3]);
	W[x4] = wasm_v128_xor(W[x4], W[x0]);
	W[x0] = wasm_v128_and(W[x0], W[x2]);
	W[x0] = wasm_v128_xor(W[x0], W[x1]);
	W[x1] = wasm_v128_and(W[x1], W[x2]);
	W[x3] = wasm_v128_xor(W[x3], W[x2]);
	W[x4] = wasm_v128_xor(W[x4], W[x3]);
	W[x2] = wasm_v128_and(W[x2], W[x3]);
	W[x3] = wasm_v128_or(W[x3], W[x0]);
	W[x1] = wasm_v128_xor(W[x1], W[x4]);
	W[x3] = wasm_v128_xor(W[x3], W[x4]);
	W[x4] = wasm_v128_and(W[x4], W[x0]);
	W[x4] = wasm_v128_xor(W[x4], W[x2]);
}

/* ── Key XOR (v128) — splat scalar subkey to all 4 lanes ─────────────────  */

static inline void keyXor_v(v128_t *W, const uint32_t *sk,
                            int a, int b, int c, int d, int i) {
	W[a] = wasm_v128_xor(W[a], wasm_i32x4_splat((int32_t)sk[4*i+0]));
	W[b] = wasm_v128_xor(W[b], wasm_i32x4_splat((int32_t)sk[4*i+1]));
	W[c] = wasm_v128_xor(W[c], wasm_i32x4_splat((int32_t)sk[4*i+2]));
	W[d] = wasm_v128_xor(W[d], wasm_i32x4_splat((int32_t)sk[4*i+3]));
}

/* ── Linear transform + key XOR (v128, encrypt) ──────────────────────────  */
/* Rotation amounts: 13, 3, 1, 7, 5, 22 — Serpent spec §3.1.               */

static inline void lk_v(v128_t *W, const uint32_t *sk,
                        int a, int b, int c, int d, int e, int i) {
	W[a] = rotl_v(W[a], 13);
	W[c] = rotl_v(W[c],  3);
	W[b] = wasm_v128_xor(W[b], W[a]);
	W[e] = wasm_i32x4_shl(W[a], 3);
	W[d] = wasm_v128_xor(W[d], W[c]);
	W[b] = wasm_v128_xor(W[b], W[c]);
	W[b] = rotl_v(W[b],  1);
	W[d] = wasm_v128_xor(W[d], W[e]);
	W[d] = rotl_v(W[d],  7);
	W[e] = W[b];
	W[a] = wasm_v128_xor(W[a], W[b]);
	W[e] = wasm_i32x4_shl(W[e], 7);
	W[c] = wasm_v128_xor(W[c], W[d]);
	W[a] = wasm_v128_xor(W[a], W[d]);
	W[c] = wasm_v128_xor(W[c], W[e]);
	W[d] = wasm_v128_xor(W[d], wasm_i32x4_splat((int32_t)sk[4*i+3]));
	W[b] = wasm_v128_xor(W[b], wasm_i32x4_splat((int32_t)sk[4*i+1]));
	W[a] = rotl_v(W[a],  5);
	W[c] = rotl_v(W[c], 22);
	W[a] = wasm_v128_xor(W[a], wasm_i32x4_splat((int32_t)sk[4*i+0]));
	W[c] = wasm_v128_xor(W[c], wasm_i32x4_splat((int32_t)sk[4*i+2]));
}

/* ── Inverse linear transform + key XOR (v128, decrypt) ──────────────────  */
/* Rotation amounts: 27, 10, 31, 25, 19, 29 — Serpent spec §3.1.           */

static inline void kl_v(v128_t *W, const uint32_t *sk,
                        int a, int b, int c, int d, int e, int i) {
	W[a] = wasm_v128_xor(W[a], wasm_i32x4_splat((int32_t)sk[4*i+0]));
	W[b] = wasm_v128_xor(W[b], wasm_i32x4_splat((int32_t)sk[4*i+1]));
	W[c] = wasm_v128_xor(W[c], wasm_i32x4_splat((int32_t)sk[4*i+2]));
	W[d] = wasm_v128_xor(W[d], wasm_i32x4_splat((int32_t)sk[4*i+3]));
	W[a] = rotl_v(W[a], 27);
	W[c] = rotl_v(W[c], 10);
	W[e] = W[b];
	W[c] = wasm_v128_xor(W[c], W[d]);
	W[a] = wasm_v128_xor(W[a], W[d]);
	W[e] = wasm_i32x4_shl(W[e], 7);
	W[a] = wasm_v128_xor(W[a], W[b]);
	W[b] = rotl_v(W[b], 31);
	W[c] = wasm_v128_xor(W[c], W[e]);
	W[d] = rotl_v(W[d], 25);
	W[e] = wasm_i32x4_shl(W[a], 3);
	W[b] = wasm_v128_xor(W[b], W[a]);
	W[d] = wasm_v128_xor(W[d], W[e]);
	W[a] = rotl_v(W[a], 19);
	W[b] = wasm_v128_xor(W[b], W[c]);
	W[d] = wasm_v128_xor(W[d], W[c]);
	W[c] = rotl_v(W[c], 29);
}

/* ── Encrypt 4 blocks (v128) ──────────────────────────────────────────────  */
/*                                                                            */
/* On entry, W[0..3] are the 4 interleaved input words:                     */
/*   W[w][lane] = word w of block lane, for w=0..3, lane=0..3.              */
/* W[4] is scratch and must be allocated but need not be initialised.       */
/* On return, W[0..3] contain the encrypted output in the same layout.      */
/* sk: the 132-word (33 × 4) subkey array from serpent_keyschedule().       */
void serpent_encrypt_4x(const uint32_t *sk, v128_t *W) {
	keyXor_v(W, sk, 0,1,2,3, 0); /* K(0) */

	/* round 0: sb0_v */
	sb0_v(W, 0,1,2,3,4);
	lk_v(W, sk, 2,1,3,0,4, 1);

	/* round 1: sb1_v */
	sb1_v(W, 2,1,3,0,4);
	lk_v(W, sk, 4,3,0,2,1, 2);

	/* round 2: sb2_v */
	sb2_v(W, 4,3,0,2,1);
	lk_v(W, sk, 1,3,4,2,0, 3);

	/* round 3: sb3_v */
	sb3_v(W, 1,3,4,2,0);
	lk_v(W, sk, 2,0,3,1,4, 4);

	/* round 4: sb4_v */
	sb4_v(W, 2,0,3,1,4);
	lk_v(W, sk, 0,3,1,4,2, 5);

	/* round 5: sb5_v */
	sb5_v(W, 0,3,1,4,2);
	lk_v(W, sk, 2,0,3,4,1, 6);

	/* round 6: sb6_v */
	sb6_v(W, 2,0,3,4,1);
	lk_v(W, sk, 3,1,0,4,2, 7);

	/* round 7: sb7_v */
	sb7_v(W, 3,1,0,4,2);
	lk_v(W, sk, 2,0,4,3,1, 8);

	/* round 8: sb0_v */
	sb0_v(W, 2,0,4,3,1);
	lk_v(W, sk, 4,0,3,2,1, 9);

	/* round 9: sb1_v */
	sb1_v(W, 4,0,3,2,1);
	lk_v(W, sk, 1,3,2,4,0, 10);

	/* round 10: sb2_v */
	sb2_v(W, 1,3,2,4,0);
	lk_v(W, sk, 0,3,1,4,2, 11);

	/* round 11: sb3_v */
	sb3_v(W, 0,3,1,4,2);
	lk_v(W, sk, 4,2,3,0,1, 12);

	/* round 12: sb4_v */
	sb4_v(W, 4,2,3,0,1);
	lk_v(W, sk, 2,3,0,1,4, 13);

	/* round 13: sb5_v */
	sb5_v(W, 2,3,0,1,4);
	lk_v(W, sk, 4,2,3,1,0, 14);

	/* round 14: sb6_v */
	sb6_v(W, 4,2,3,1,0);
	lk_v(W, sk, 3,0,2,1,4, 15);

	/* round 15: sb7_v */
	sb7_v(W, 3,0,2,1,4);
	lk_v(W, sk, 4,2,1,3,0, 16);

	/* round 16: sb0_v */
	sb0_v(W, 4,2,1,3,0);
	lk_v(W, sk, 1,2,3,4,0, 17);

	/* round 17: sb1_v */
	sb1_v(W, 1,2,3,4,0);
	lk_v(W, sk, 0,3,4,1,2, 18);

	/* round 18: sb2_v */
	sb2_v(W, 0,3,4,1,2);
	lk_v(W, sk, 2,3,0,1,4, 19);

	/* round 19: sb3_v */
	sb3_v(W, 2,3,0,1,4);
	lk_v(W, sk, 1,4,3,2,0, 20);

	/* round 20: sb4_v */
	sb4_v(W, 1,4,3,2,0);
	lk_v(W, sk, 4,3,2,0,1, 21);

	/* round 21: sb5_v */
	sb5_v(W, 4,3,2,0,1);
	lk_v(W, sk, 1,4,3,0,2, 22);

	/* round 22: sb6_v */
	sb6_v(W, 1,4,3,0,2);
	lk_v(W, sk, 3,2,4,0,1, 23);

	/* round 23: sb7_v */
	sb7_v(W, 3,2,4,0,1);
	lk_v(W, sk, 1,4,0,3,2, 24);

	/* round 24: sb0_v */
	sb0_v(W, 1,4,0,3,2);
	lk_v(W, sk, 0,4,3,1,2, 25);

	/* round 25: sb1_v */
	sb1_v(W, 0,4,3,1,2);
	lk_v(W, sk, 2,3,1,0,4, 26);

	/* round 26: sb2_v */
	sb2_v(W, 2,3,1,0,4);
	lk_v(W, sk, 4,3,2,0,1, 27);

	/* round 27: sb3_v */
	sb3_v(W, 4,3,2,0,1);
	lk_v(W, sk, 0,1,3,4,2, 28);

	/* round 28: sb4_v */
	sb4_v(W, 0,1,3,4,2);
	lk_v(W, sk, 1,3,4,2,0, 29);

	/* round 29: sb5_v */
	sb5_v(W, 1,3,4,2,0);
	lk_v(W, sk, 0,1,3,2,4, 30);

	/* round 30: sb6_v */
	sb6_v(W, 0,1,3,2,4);
	lk_v(W, sk, 3,4,1,2,0, 31);

	/* round 31: sb7_v (final — no linear transform) */
	sb7_v(W, 3,4,1,2,0);

	keyXor_v(W, sk, 0,1,2,3, 32); /* K(32) */
}

/* ── Decrypt 4 blocks (v128) ──────────────────────────────────────────────  */
/*                                                                            */
/* Same interleaved input layout as encrypt.                                 */
/* Note: on return the output words are in W[4,1,3,2] not W[0,1,2,3].      */
/* This matches the scalar decrypt register layout. The CTR path always      */
/* uses encrypt_4x (CTR is symmetric), so this mainly matters for CBC.      */
void serpent_decrypt_4x(const uint32_t *sk, v128_t *W) {
	keyXor_v(W, sk, 0,1,2,3, 32); /* K(32) */

	/* round 0: si7_v */
	si7_v(W, 0,1,2,3,4);
	kl_v(W, sk, 1,3,0,4,2, 31);

	/* round 1: si6_v */
	si6_v(W, 1,3,0,4,2);
	kl_v(W, sk, 0,2,4,1,3, 30);

	/* round 2: si5_v */
	si5_v(W, 0,2,4,1,3);
	kl_v(W, sk, 2,3,0,4,1, 29);

	/* round 3: si4_v */
	si4_v(W, 2,3,0,4,1);
	kl_v(W, sk, 2,0,1,4,3, 28);

	/* round 4: si3_v */
	si3_v(W, 2,0,1,4,3);
	kl_v(W, sk, 1,2,3,4,0, 27);

	/* round 5: si2_v */
	si2_v(W, 1,2,3,4,0);
	kl_v(W, sk, 2,0,4,3,1, 26);

	/* round 6: si1_v */
	si1_v(W, 2,0,4,3,1);
	kl_v(W, sk, 1,0,4,3,2, 25);

	/* round 7: si0_v */
	si0_v(W, 1,0,4,3,2);
	kl_v(W, sk, 4,2,0,1,3, 24);

	/* round 8: si7_v */
	si7_v(W, 4,2,0,1,3);
	kl_v(W, sk, 2,1,4,3,0, 23);

	/* round 9: si6_v */
	si6_v(W, 2,1,4,3,0);
	kl_v(W, sk, 4,0,3,2,1, 22);

	/* round 10: si5_v */
	si5_v(W, 4,0,3,2,1);
	kl_v(W, sk, 0,1,4,3,2, 21);

	/* round 11: si4_v */
	si4_v(W, 0,1,4,3,2);
	kl_v(W, sk, 0,4,2,3,1, 20);

	/* round 12: si3_v */
	si3_v(W, 0,4,2,3,1);
	kl_v(W, sk, 2,0,1,3,4, 19);

	/* round 13: si2_v */
	si2_v(W, 2,0,1,3,4);
	kl_v(W, sk, 0,4,3,1,2, 18);

	/* round 14: si1_v */
	si1_v(W, 0,4,3,1,2);
	kl_v(W, sk, 2,4,3,1,0, 17);

	/* round 15: si0_v */
	si0_v(W, 2,4,3,1,0);
	kl_v(W, sk, 3,0,4,2,1, 16);

	/* round 16: si7_v */
	si7_v(W, 3,0,4,2,1);
	kl_v(W, sk, 0,2,3,1,4, 15);

	/* round 17: si6_v */
	si6_v(W, 0,2,3,1,4);
	kl_v(W, sk, 3,4,1,0,2, 14);

	/* round 18: si5_v */
	si5_v(W, 3,4,1,0,2);
	kl_v(W, sk, 4,2,3,1,0, 13);

	/* round 19: si4_v */
	si4_v(W, 4,2,3,1,0);
	kl_v(W, sk, 4,3,0,1,2, 12);

	/* round 20: si3_v */
	si3_v(W, 4,3,0,1,2);
	kl_v(W, sk, 0,4,2,1,3, 11);

	/* round 21: si2_v */
	si2_v(W, 0,4,2,1,3);
	kl_v(W, sk, 4,3,1,2,0, 10);

	/* round 22: si1_v */
	si1_v(W, 4,3,1,2,0);
	kl_v(W, sk, 0,3,1,2,4, 9);

	/* round 23: si0_v */
	si0_v(W, 0,3,1,2,4);
	kl_v(W, sk, 1,4,3,0,2, 8);

	/* round 24: si7_v */
	si7_v(W, 1,4,3,0,2);
	kl_v(W, sk, 4,0,1,2,3, 7);

	/* round 25: si6_v */
	si6_v(W, 4,0,1,2,3);
	kl_v(W, sk, 1,3,2,4,0, 6);

	/* round 26: si5_v */
	si5_v(W, 1,3,2,4,0);
	kl_v(W, sk, 3,0,1,2,4, 5);

	/* round 27: si4_v */
	si4_v(W, 3,0,1,2,4);
	kl_v(W, sk, 3,1,4,2,0, 4);

	/* round 28: si3_v */
	si3_v(W, 3,1,4,2,0);
	kl_v(W, sk, 4,3,0,2,1, 3);

	/* round 29: si2_v */
	si2_v(W, 4,3,0,2,1);
	kl_v(W, sk, 3,1,2,0,4, 2);

	/* round 30: si1_v */
	si1_v(W, 3,1,2,0,4);
	kl_v(W, sk, 4,1,2,0,3, 1);

	/* round 31: si0_v (final — no inverse linear transform) */
	si0_v(W, 4,1,2,0,3);

	/* K(0): final key XOR — output slots are (4,1,3,2), not (0,1,2,3) */
	keyXor_v(W, sk, 4,1,3,2, 0);
}

#endif /* __wasm_simd128__ */
