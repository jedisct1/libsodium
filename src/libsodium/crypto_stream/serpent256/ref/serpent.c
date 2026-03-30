/*
 * src/ref/serpent.c
 *
 * Serpent-256 block cipher — scalar C reference implementation.
 *
 * Ported from leviathan-crypto/src/asm/serpent/serpent.ts (AssemblyScript)
 * which was itself ported from the Serpent AES submission sources (floppy1).
 * Byte ordering and key schedule follow the leviathan-crypto convention,
 * confirmed correct against the NESSIE, KAT, and CTR test vector suites.
 *
 * Security properties:
 *   - S-boxes are Boolean circuits (AND/OR/XOR/NOT on uint32_t only).
 *     No table lookups. No data-dependent memory access. Constant-time.
 *   - Key schedule uses only arithmetic and bitwise ops. No secret branches.
 *   - Working registers are stack variables; sodium_memzero() wipes them
 *     and the subkey array on request.
 *   - Round count (32) is a structural constant. No reduced-round risk.
 *   - Best known attack reaches 12 of 32 rounds (20-round security margin).
 */

#include "serpent.h"
#include "utils.h"   /* sodium_memzero */

#include <string.h>
#include <stdint.h>

/* ── Helpers ──────────────────────────────────────────────────────────────── */

#define ROTL32(v, n) (((uint32_t)(v) << (n)) | ((uint32_t)(v) >> (32 - (n))))

/* Load/store helpers — avoid UB from unaligned access on strict-alignment
 * architectures. These compile to a single instruction on x86/ARM. */
static inline void store8_be32(uint8_t *p, uint32_t v) {
	p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16);
	p[2] = (uint8_t)(v >>  8); p[3] = (uint8_t) v;
}

/* ── Round-constant tables ────────────────────────────────────────────────── */
/*
 * CRT-encoded slot permutations for S-box dispatch and subkey storage.
 * Each value m encodes a permutation (m%5, m%7, m%11, m%13, m%17) of
 * {0,1,2,3,4}.  The CRT guarantee ensures all five residues are distinct
 * and in {0..4}, so they safely index the 5-element working register array.
 *
 * EC[n]: slot permutation for encryption round n (S-box application then lk).
 * DC[n]: slot permutation for decryption round n (inverse S-box then kl).
 * KC[n]: slot permutation for key schedule S-box application and subkey store.
 *
 * Identical to ec/dc/kc in leviathan-crypto/src/asm/serpent/serpent.ts.
 */
static const uint32_t EC[32] = {
	44255, 61867, 45034, 52496, 73087, 56255, 43827, 41448,
	18242,  1939, 18581, 56255, 64584, 31097, 26469, 77728,
	77639,  4216, 64585, 31097, 66861, 78949, 58006, 59943,
	49676, 78950,  5512, 78949, 27525, 52496, 18670, 76143,
};
static const uint32_t DC[32] = {
	44255, 60896, 28835,  1837,  1057,  4216, 18242, 77301,
	47399, 53992,  1939,  1940, 66420, 39172, 78950, 45917,
	82383,  7450, 67288, 26469, 83149, 57565, 66419, 47400,
	58006, 44254, 18581, 18228, 33048, 45034, 66508,  7449,
};
static const uint32_t KC[34] = {
	 7788, 63716, 84032,  7891, 78949, 25146, 28835, 67288,
	84032, 40055,  7361,  1940, 77639, 27525, 24193, 75702,
	 7361, 35413, 83150, 82383, 58619, 48468, 18242, 66861,
	83150, 69667,  7788, 31552, 40054, 23222, 52496, 57565,
	 7788, 63716,
};

/* ── Forward S-boxes ─────────────────────────────────────────────────────── */
/*
 * Boolean circuit implementations of the 8 Serpent S-boxes.
 * Each operates on r[x0..x4] where x0..x4 are distinct indices in {0..4}.
 * All gates are AND/OR/XOR/NOT on uint32_t — constant-time by construction.
 * Derived from leviathan-crypto/src/asm/serpent/serpent.ts sb0..sb7.
 */

#define R(i) r[(i)]

static inline void sb0(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x3);
	R(x3) |= R(x0);
	R(x0) ^= R(x4);
	R(x4) ^= R(x2);
	R(x4)  = ~R(x4);
	R(x3) ^= R(x1);
	R(x1) &= R(x0);
	R(x1) ^= R(x4);
	R(x2) ^= R(x0);
	R(x0) ^= R(x3);
	R(x4) |= R(x0);
	R(x0) ^= R(x2);
	R(x2) &= R(x1);
	R(x3) ^= R(x2);
	R(x1)  = ~R(x1);
	R(x2) ^= R(x4);
	R(x1) ^= R(x2);
}

static inline void sb1(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x1);
	R(x1) ^= R(x0);
	R(x0) ^= R(x3);
	R(x3)  = ~R(x3);
	R(x4) &= R(x1);
	R(x0) |= R(x1);
	R(x3) ^= R(x2);
	R(x0) ^= R(x3);
	R(x1) ^= R(x3);
	R(x3) ^= R(x4);
	R(x1) |= R(x4);
	R(x4) ^= R(x2);
	R(x2) &= R(x0);
	R(x2) ^= R(x1);
	R(x1) |= R(x0);
	R(x0)  = ~R(x0);
	R(x0) ^= R(x2);
	R(x4) ^= R(x1);
}

static inline void sb2(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x3)  = ~R(x3);
	R(x1) ^= R(x0);
	R(x4) = R(x0);
	R(x0) &= R(x2);
	R(x0) ^= R(x3);
	R(x3) |= R(x4);
	R(x2) ^= R(x1);
	R(x3) ^= R(x1);
	R(x1) &= R(x0);
	R(x0) ^= R(x2);
	R(x2) &= R(x3);
	R(x3) |= R(x1);
	R(x0)  = ~R(x0);
	R(x3) ^= R(x0);
	R(x4) ^= R(x0);
	R(x0) ^= R(x2);
	R(x1) |= R(x2);
}

static inline void sb3(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x1);
	R(x1) ^= R(x3);
	R(x3) |= R(x0);
	R(x4) &= R(x0);
	R(x0) ^= R(x2);
	R(x2) ^= R(x1);
	R(x1) &= R(x3);
	R(x2) ^= R(x3);
	R(x0) |= R(x4);
	R(x4) ^= R(x3);
	R(x1) ^= R(x0);
	R(x0) &= R(x3);
	R(x3) &= R(x4);
	R(x3) ^= R(x2);
	R(x4) |= R(x1);
	R(x2) &= R(x1);
	R(x4) ^= R(x3);
	R(x0) ^= R(x3);
	R(x3) ^= R(x2);
}

static inline void sb4(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x3);
	R(x3) &= R(x0);
	R(x0) ^= R(x4);
	R(x3) ^= R(x2);
	R(x2) |= R(x4);
	R(x0) ^= R(x1);
	R(x4) ^= R(x3);
	R(x2) |= R(x0);
	R(x2) ^= R(x1);
	R(x1) &= R(x0);
	R(x1) ^= R(x4);
	R(x4) &= R(x2);
	R(x2) ^= R(x3);
	R(x4) ^= R(x0);
	R(x3) |= R(x1);
	R(x1)  = ~R(x1);
	R(x3) ^= R(x0);
}

static inline void sb5(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x1);
	R(x1) |= R(x0);
	R(x2) ^= R(x1);
	R(x3)  = ~R(x3);
	R(x4) ^= R(x0);
	R(x0) ^= R(x2);
	R(x1) &= R(x4);
	R(x4) |= R(x3);
	R(x4) ^= R(x0);
	R(x0) &= R(x3);
	R(x1) ^= R(x3);
	R(x3) ^= R(x2);
	R(x0) ^= R(x1);
	R(x2) &= R(x4);
	R(x1) ^= R(x2);
	R(x2) &= R(x0);
	R(x3) ^= R(x2);
}

static inline void sb6(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x1);
	R(x3) ^= R(x0);
	R(x1) ^= R(x2);
	R(x2) ^= R(x0);
	R(x0) &= R(x3);
	R(x1) |= R(x3);
	R(x4)  = ~R(x4);
	R(x0) ^= R(x1);
	R(x1) ^= R(x2);
	R(x3) ^= R(x4);
	R(x4) ^= R(x0);
	R(x2) &= R(x0);
	R(x4) ^= R(x1);
	R(x2) ^= R(x3);
	R(x3) &= R(x1);
	R(x3) ^= R(x0);
	R(x1) ^= R(x2);
}

static inline void sb7(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x1)  = ~R(x1);
	R(x4) = R(x1);
	R(x0)  = ~R(x0);
	R(x1) &= R(x2);
	R(x1) ^= R(x3);
	R(x3) |= R(x4);
	R(x4) ^= R(x2);
	R(x2) ^= R(x3);
	R(x3) ^= R(x0);
	R(x0) |= R(x1);
	R(x2) &= R(x0);
	R(x0) ^= R(x4);
	R(x4) ^= R(x3);
	R(x3) &= R(x0);
	R(x4) ^= R(x1);
	R(x2) ^= R(x4);
	R(x3) ^= R(x1);
	R(x4) |= R(x0);
	R(x4) ^= R(x1);
}

/* ── Inverse S-boxes ─────────────────────────────────────────────────────── */
/* Derived from leviathan-crypto/src/asm/serpent/serpent.ts si0..si7.        */

static inline void si0(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x3);
	R(x1) ^= R(x0);
	R(x3) |= R(x1);
	R(x4) ^= R(x1);
	R(x0)  = ~R(x0);
	R(x2) ^= R(x3);
	R(x3) ^= R(x0);
	R(x0) &= R(x1);
	R(x0) ^= R(x2);
	R(x2) &= R(x3);
	R(x3) ^= R(x4);
	R(x2) ^= R(x3);
	R(x1) ^= R(x3);
	R(x3) &= R(x0);
	R(x1) ^= R(x0);
	R(x0) ^= R(x2);
	R(x4) ^= R(x3);
}

static inline void si1(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x1) ^= R(x3);
	R(x4) = R(x0);
	R(x0) ^= R(x2);
	R(x2)  = ~R(x2);
	R(x4) |= R(x1);
	R(x4) ^= R(x3);
	R(x3) &= R(x1);
	R(x1) ^= R(x2);
	R(x2) &= R(x4);
	R(x4) ^= R(x1);
	R(x1) |= R(x3);
	R(x3) ^= R(x0);
	R(x2) ^= R(x0);
	R(x0) |= R(x4);
	R(x2) ^= R(x4);
	R(x1) ^= R(x0);
	R(x4) ^= R(x1);
}

static inline void si2(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x2) ^= R(x1);
	R(x4) = R(x3);
	R(x3)  = ~R(x3);
	R(x3) |= R(x2);
	R(x2) ^= R(x4);
	R(x4) ^= R(x0);
	R(x3) ^= R(x1);
	R(x1) |= R(x2);
	R(x2) ^= R(x0);
	R(x1) ^= R(x4);
	R(x4) |= R(x3);
	R(x2) ^= R(x3);
	R(x4) ^= R(x2);
	R(x2) &= R(x1);
	R(x2) ^= R(x3);
	R(x3) ^= R(x4);
	R(x4) ^= R(x0);
}

static inline void si3(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x2) ^= R(x1);
	R(x4) = R(x1);
	R(x1) &= R(x2);
	R(x1) ^= R(x0);
	R(x0) |= R(x4);
	R(x4) ^= R(x3);
	R(x0) ^= R(x3);
	R(x3) |= R(x1);
	R(x1) ^= R(x2);
	R(x1) ^= R(x3);
	R(x0) ^= R(x2);
	R(x2) ^= R(x3);
	R(x3) &= R(x1);
	R(x1) ^= R(x0);
	R(x0) &= R(x2);
	R(x4) ^= R(x3);
	R(x3) ^= R(x0);
	R(x0) ^= R(x1);
}

static inline void si4(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x2) ^= R(x3);
	R(x4) = R(x0);
	R(x0) &= R(x1);
	R(x0) ^= R(x2);
	R(x2) |= R(x3);
	R(x4)  = ~R(x4);
	R(x1) ^= R(x0);
	R(x0) ^= R(x2);
	R(x2) &= R(x4);
	R(x2) ^= R(x0);
	R(x0) |= R(x4);
	R(x0) ^= R(x3);
	R(x3) &= R(x2);
	R(x4) ^= R(x3);
	R(x3) ^= R(x1);
	R(x1) &= R(x0);
	R(x4) ^= R(x1);
	R(x0) ^= R(x3);
}

static inline void si5(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x1);
	R(x1) |= R(x2);
	R(x2) ^= R(x4);
	R(x1) ^= R(x3);
	R(x3) &= R(x4);
	R(x2) ^= R(x3);
	R(x3) |= R(x0);
	R(x0)  = ~R(x0);
	R(x3) ^= R(x2);
	R(x2) |= R(x0);
	R(x4) ^= R(x1);
	R(x2) ^= R(x4);
	R(x4) &= R(x0);
	R(x0) ^= R(x1);
	R(x1) ^= R(x3);
	R(x0) &= R(x2);
	R(x2) ^= R(x3);
	R(x0) ^= R(x2);
	R(x2) ^= R(x4);
	R(x4) ^= R(x3);
}

static inline void si6(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x0) ^= R(x2);
	R(x4) = R(x0);
	R(x0) &= R(x3);
	R(x2) ^= R(x3);
	R(x0) ^= R(x2);
	R(x3) ^= R(x1);
	R(x2) |= R(x4);
	R(x2) ^= R(x3);
	R(x3) &= R(x0);
	R(x0)  = ~R(x0);
	R(x3) ^= R(x1);
	R(x1) &= R(x2);
	R(x4) ^= R(x0);
	R(x3) ^= R(x4);
	R(x4) ^= R(x2);
	R(x0) ^= R(x1);
	R(x2) ^= R(x0);
}

static inline void si7(uint32_t *r, int x0, int x1, int x2, int x3, int x4) {
	R(x4) = R(x3);
	R(x3) &= R(x0);
	R(x0) ^= R(x2);
	R(x2) |= R(x4);
	R(x4) ^= R(x1);
	R(x0)  = ~R(x0);
	R(x1) |= R(x3);
	R(x4) ^= R(x0);
	R(x0) &= R(x2);
	R(x0) ^= R(x1);
	R(x1) &= R(x2);
	R(x3) ^= R(x2);
	R(x4) ^= R(x3);
	R(x2) &= R(x3);
	R(x3) |= R(x0);
	R(x1) ^= R(x4);
	R(x3) ^= R(x4);
	R(x4) &= R(x0);
	R(x4) ^= R(x2);
}

/* ── S-box dispatch ──────────────────────────────────────────────────────── */

static inline void
apply_sb(uint32_t *r, int idx, int x0, int x1, int x2, int x3, int x4) {
	switch (idx) {
	case 0: sb0(r,x0,x1,x2,x3,x4); break;
	case 1: sb1(r,x0,x1,x2,x3,x4); break;
	case 2: sb2(r,x0,x1,x2,x3,x4); break;
	case 3: sb3(r,x0,x1,x2,x3,x4); break;
	case 4: sb4(r,x0,x1,x2,x3,x4); break;
	case 5: sb5(r,x0,x1,x2,x3,x4); break;
	case 6: sb6(r,x0,x1,x2,x3,x4); break;
	case 7: sb7(r,x0,x1,x2,x3,x4); break;
	}
}

static inline void
apply_si(uint32_t *r, int idx, int x0, int x1, int x2, int x3, int x4) {
	switch (idx) {
	case 0: si0(r,x0,x1,x2,x3,x4); break;
	case 1: si1(r,x0,x1,x2,x3,x4); break;
	case 2: si2(r,x0,x1,x2,x3,x4); break;
	case 3: si3(r,x0,x1,x2,x3,x4); break;
	case 4: si4(r,x0,x1,x2,x3,x4); break;
	case 5: si5(r,x0,x1,x2,x3,x4); break;
	case 6: si6(r,x0,x1,x2,x3,x4); break;
	case 7: si7(r,x0,x1,x2,x3,x4); break;
	}
}

/* ── Linear transforms ───────────────────────────────────────────────────── */
/*
 * lk — forward linear transform followed by round-key XOR (encryption).
 * Rotation amounts: 13, 3, 1, 7, 5, 22 — Serpent spec §3.1.
 * Applied after every S-box except the last (round 31).
 */
static inline void
lk(uint32_t *r, const uint32_t *sk,
   int a, int b, int c, int d, int e, int i)
{
	R(a) = ROTL32(R(a), 13);
	R(c) = ROTL32(R(c),  3);
	R(b) ^= R(a);
	R(e)  = R(a) << 3;
	R(d) ^= R(c);
	R(b) ^= R(c);
	R(b) = ROTL32(R(b),  1);
	R(d) ^= R(e);
	R(d) = ROTL32(R(d),  7);
	R(e)  = R(b);
	R(a) ^= R(b);
	R(e) <<= 7;
	R(c) ^= R(d);
	R(a) ^= R(d);
	R(c) ^= R(e);
	R(d) ^= sk[4*i+3];
	R(b) ^= sk[4*i+1];
	R(a) = ROTL32(R(a),  5);
	R(c) = ROTL32(R(c), 22);
	R(a) ^= sk[4*i+0];
	R(c) ^= sk[4*i+2];
}

/*
 * kl — round-key XOR followed by inverse linear transform (decryption).
 * Rotation amounts: 27, 10, 31, 25, 19, 29 — Serpent spec §3.1.
 * Applied after every inverse S-box except the last (round 31).
 */
static inline void
kl(uint32_t *r, const uint32_t *sk,
   int a, int b, int c, int d, int e, int i)
{
	R(a) ^= sk[4*i+0];
	R(b) ^= sk[4*i+1];
	R(c) ^= sk[4*i+2];
	R(d) ^= sk[4*i+3];
	R(a) = ROTL32(R(a), 27);
	R(c) = ROTL32(R(c), 10);
	R(e)  = R(b);
	R(c) ^= R(d);
	R(a) ^= R(d);
	R(e) <<= 7;
	R(a) ^= R(b);
	R(b) = ROTL32(R(b), 31);
	R(c) ^= R(e);
	R(d) = ROTL32(R(d), 25);
	R(e)  = R(a) << 3;
	R(b) ^= R(a);
	R(d) ^= R(e);
	R(a) = ROTL32(R(a), 19);
	R(b) ^= R(c);
	R(d) ^= R(c);
	R(c) = ROTL32(R(c), 29);
}

/* ── Key schedule ────────────────────────────────────────────────────────── */
/*
 * Byte ordering: input key bytes are reverse-copied to a byte array (padding
 * shorter keys to 256 bits per the Serpent spec), repacked as 8 LE uint32
 * words, then expanded via the affine recurrence to 132 prekey words.
 * The 33 round keys (K0..K32) are derived by applying the bitslice S-boxes
 * in the order given by KC[], working backwards from prekey words 131..0.
 *
 * After serpent_keyschedule():
 *   sk[0..3]   = K0  (XORed before round 0)
 *   sk[4..7]   = K1  (lk after round 0)
 *   ...
 *   sk[128..131] = K32 (XORed after round 31)
 */
int serpent_keyschedule(const uint8_t *key, int keylen, uint32_t sk[132])
{
	uint32_t r[5];
	int      ii, jj, n, ri, rj;
	uint32_t m;

	if (keylen != 16 && keylen != 24 && keylen != 32)
		return -1;

	/* Zero the entire subkey buffer (handles padding for short keys). */
	memset(sk, 0, 132 * sizeof(uint32_t));

	/*
	 * Build 8 user-key words.
	 *
	 * The Serpent spec pads shorter keys by appending a 1-bit then zeros.
	 * In the byte-array representation before repacking:
	 *   byte[k] = key[keylen-1-k]  for k in 0..keylen-1  (byte reversal)
	 *   byte[keylen] = 1            (padding bit)
	 *   byte[keylen+1..31] = 0      (already zeroed above)
	 *
	 * Each group of 4 bytes is packed as a little-endian uint32:
	 *   sk[w] = byte[4w] | byte[4w+1]<<8 | byte[4w+2]<<16 | byte[4w+3]<<24
	 *
	 * Equivalent (unrolling the reverse + repack):
	 *   sk[w] = key[keylen-1-4w]  | key[keylen-2-4w]<<8
	 *         | key[keylen-3-4w]<<16 | key[keylen-4-4w]<<24
	 *
	 * For full 256-bit keys (keylen=32) the padding word is at sk[8] = 0
	 * (already handled by memset above).
	 * For 128/192-bit keys the padding byte lands in one of sk[0..7].
	 */
	{
		/*
		 * Expand key bytes individually into a temporary uint32 array
		 * (one byte per word, same layout as the AS implementation's
		 * byte-valued i32 array before the repack step).
		 */
		uint32_t tmp[32];
		int      k;

		memset(tmp, 0, sizeof tmp);
		tmp[keylen] = 1; /* padding bit */
		for (k = 0; k < keylen; k++)
			tmp[k] = key[keylen - 1 - k];

		/* Repack: 4 byte-valued uint32s → 1 LE uint32. */
		for (k = 0; k < 8; k++) {
			sk[k] = (tmp[4*k+0]       )
			      | (tmp[4*k+1] <<  8)
			      | (tmp[4*k+2] << 16)
			      | (tmp[4*k+3] << 24);
		}
	}

	/*
	 * Prekey expansion.
	 *
	 * Implements the affine recurrence:
	 *   w[i] = rotl(w[i-8] ^ w[i-5] ^ w[i-3] ^ w[i-1] ^ phi ^ i, 11)
	 *
	 * sk[0..7] hold the initial key words (w[-8..−1] in the spec).
	 * The recurrence overwrites sk[0..131] in-place with w[0..131].
	 * This is safe because w[i] only depends on w[i-8..i-1], so the
	 * source word at index i-8 is always read before being overwritten.
	 *
	 * Working registers r[0..4] are a 5-element sliding window used to
	 * avoid redundant loads. r[b] is updated to the newly computed word
	 * at each step, matching the AS keyIt() helper.
	 *
	 * The loop body groups 5 keyIt calls per iteration with the "j=i-8"
	 * read offset tracked by jj.  jj wraps to 0 when ii reaches 8 so
	 * that the wrap-around from the 8-element key window works correctly.
	 */
	r[0] = sk[3]; r[1] = sk[4]; r[2] = sk[5]; r[3] = sk[6]; r[4] = sk[7];

#define KEYIT(a, b, c, d, i) do {                                   \
	uint32_t _v = ROTL32(sk[(a)] ^ r[(b)] ^ r[(c)] ^ r[(d)]    \
	                     ^ 0x9e3779b9u ^ (uint32_t)(i), 11);    \
	sk[(i)] = _v;                                                \
	r[(b)]  = _v;                                                \
} while (0)

	ii = 0; jj = 0;
	for (;;) {
		int ja = jj++;
		KEYIT(ja, 0, 4, 2, ii); ii++;
		int jb = jj++;
		KEYIT(jb, 1, 0, 3, ii); ii++;
		if (ii >= 132) break;
		int jc = jj++;
		KEYIT(jc, 2, 1, 4, ii); ii++;
		if (ii == 8) jj = 0;
		int jd = jj++;
		KEYIT(jd, 3, 2, 0, ii); ii++;
		int je = jj++;
		KEYIT(je, 4, 3, 1, ii); ii++;
	}
#undef KEYIT

	/*
	 * Round key derivation.
	 *
	 * The 33 round keys K0..K32 are derived from the prekey words by
	 * applying one of the 8 Serpent S-boxes to successive groups of 4
	 * prekey words.  The S-box index cycles as (3,4,5,6,7,0,1,2,...) for
	 * 33 applications (rj starts at 3).  Slot assignments are given by the
	 * KC[] constants.
	 *
	 * Keys are stored in REVERSE order — K32 first (at sk[128..131]),
	 * K31 next (sk[124..127]), down to K0 (sk[0..3]) — so that after the
	 * loop sk[] is in forward encryption order (K0 first).
	 *
	 * The working registers after the prekey loop hold specific prekey
	 * words ready for the first S-box application:
	 *   r[0]=sk[130], r[1]=sk[131], r[2]=sk[127],
	 *   r[3]=sk[128], r[4]=sk[129]
	 * (verified empirically; see generate/README.md).
	 *
	 * Each iteration:
	 *   1. Apply S-box (using KC[n] slots) to current working regs.
	 *   2. Store 4 output words to sk[ri..ri+3] (using KC[n+1] slots).
	 *   3. Load next group of 4 prekey words from sk[ri-4..ri-1].
	 */
	ri = 128; rj = 3; n = 0;
	for (;;) {
		m = KC[n++];
		apply_sb(r, rj % 8, m%5, m%7, m%11, m%13, m%17);
		rj++;

		m = KC[n];
		sk[ri+0] = r[m%5];
		sk[ri+1] = r[m%7];
		sk[ri+2] = r[m%11];
		sk[ri+3] = r[m%13];

		if (ri <= 0) break;
		ri -= 4;

		r[m%5]  = sk[ri+0];
		r[m%7]  = sk[ri+1];
		r[m%11] = sk[ri+2];
		r[m%13] = sk[ri+3];
	}

	sodium_memzero(r, sizeof r);
	return 0;
}

/* ── Block encryption ────────────────────────────────────────────────────── */
/*
 * Byte ordering (matching leviathan-crypto and ctr_harness.c):
 *
 *   Load:  the 16-byte block is byte-reversed, then read as 4 LE uint32s.
 *          r[0] = LE(pt[15..12])  ← MS-word of the reversed block
 *          r[1] = LE(pt[11..8])
 *          r[2] = LE(pt[7..4])
 *          r[3] = LE(pt[3..0])   ← LS-word of the reversed block
 *
 *   Store: each register is written big-endian; r[3] → ct[0..3] (MSB first).
 *          ct[0..3]   = r[3] BE
 *          ct[4..7]   = r[2] BE
 *          ct[8..11]  = r[1] BE
 *          ct[12..15] = r[0] BE
 */
void serpent_encrypt(const uint32_t *sk, const uint8_t pt[16], uint8_t ct[16])
{
	uint32_t r[5];
	int      n;
	uint32_t m;

	/* Load plaintext. */
	r[0] = (uint32_t)pt[15] | ((uint32_t)pt[14]<<8)
	     | ((uint32_t)pt[13]<<16) | ((uint32_t)pt[12]<<24);
	r[1] = (uint32_t)pt[11] | ((uint32_t)pt[10]<<8)
	     | ((uint32_t)pt[ 9]<<16) | ((uint32_t)pt[ 8]<<24);
	r[2] = (uint32_t)pt[ 7] | ((uint32_t)pt[ 6]<<8)
	     | ((uint32_t)pt[ 5]<<16) | ((uint32_t)pt[ 4]<<24);
	r[3] = (uint32_t)pt[ 3] | ((uint32_t)pt[ 2]<<8)
	     | ((uint32_t)pt[ 1]<<16) | ((uint32_t)pt[ 0]<<24);

	/* K(0): XOR initial subkey. */
	r[0] ^= sk[0]; r[1] ^= sk[1]; r[2] ^= sk[2]; r[3] ^= sk[3];

	/* 32 rounds: S-box then linear transform + key XOR (except final). */
	n = 0; m = EC[0];
	for (;;) {
		apply_sb(r, n % 8, m%5, m%7, m%11, m%13, m%17);
		if (n >= 31) break;
		n++; m = EC[n];
		lk(r, sk, m%5, m%7, m%11, m%13, m%17, n);
	}

	/* K(32): XOR final subkey. */
	r[0] ^= sk[128]; r[1] ^= sk[129]; r[2] ^= sk[130]; r[3] ^= sk[131];

	/* Store ciphertext big-endian: r[3] → ct[0..3], ..., r[0] → ct[12..15]. */
	store8_be32(ct +  0, r[3]);
	store8_be32(ct +  4, r[2]);
	store8_be32(ct +  8, r[1]);
	store8_be32(ct + 12, r[0]);

	sodium_memzero(r, sizeof r);
}

/* ── Block decryption ────────────────────────────────────────────────────── */
/*
 * Load byte ordering is identical to encryption (same byte-reversal).
 *
 * After the 32 inverse rounds, the output words occupy r[4,1,3,2]
 * (not r[0,1,2,3]).  Store order:
 *   pt[0..3]   = r[4] BE
 *   pt[4..7]   = r[1] BE
 *   pt[8..11]  = r[3] BE
 *   pt[12..15] = r[2] BE
 */
void serpent_decrypt(const uint32_t *sk, const uint8_t ct[16], uint8_t pt[16])
{
	uint32_t r[5];
	int      n;
	uint32_t m;

	/* Load ciphertext (same byte-reversal as encrypt). */
	r[0] = (uint32_t)ct[15] | ((uint32_t)ct[14]<<8)
	     | ((uint32_t)ct[13]<<16) | ((uint32_t)ct[12]<<24);
	r[1] = (uint32_t)ct[11] | ((uint32_t)ct[10]<<8)
	     | ((uint32_t)ct[ 9]<<16) | ((uint32_t)ct[ 8]<<24);
	r[2] = (uint32_t)ct[ 7] | ((uint32_t)ct[ 6]<<8)
	     | ((uint32_t)ct[ 5]<<16) | ((uint32_t)ct[ 4]<<24);
	r[3] = (uint32_t)ct[ 3] | ((uint32_t)ct[ 2]<<8)
	     | ((uint32_t)ct[ 1]<<16) | ((uint32_t)ct[ 0]<<24);

	/* K(32): XOR final subkey. */
	r[0] ^= sk[128]; r[1] ^= sk[129]; r[2] ^= sk[130]; r[3] ^= sk[131];

	/* 32 inverse rounds: inverse S-box then inverse LT + key XOR (except final). */
	n = 0; m = DC[0];
	for (;;) {
		apply_si(r, 7 - (n % 8), m%5, m%7, m%11, m%13, m%17);
		if (n >= 31) break;
		n++; m = DC[n];
		kl(r, sk, m%5, m%7, m%11, m%13, m%17, 32 - n);
	}

	/* K(0): final key XOR — slots (2,3,1,4), not (0,1,2,3). */
	r[2] ^= sk[0]; r[3] ^= sk[1]; r[1] ^= sk[2]; r[4] ^= sk[3];

	/* Store plaintext: r[4] → pt[0..3], r[1] → pt[4..7], etc. */
	store8_be32(pt +  0, r[4]);
	store8_be32(pt +  4, r[1]);
	store8_be32(pt +  8, r[3]);
	store8_be32(pt + 12, r[2]);

	sodium_memzero(r, sizeof r);
}

/* ── Wipe ────────────────────────────────────────────────────────────────── */

void serpent_wipe(uint32_t sk[132])
{
	sodium_memzero(sk, 132 * sizeof(uint32_t));
}
