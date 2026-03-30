/*
 * src/wasm32/serpent_simd.h
 *
 * Declaration for the 4-wide WASM SIMD Serpent block cipher.
 */

#ifndef SERPENT_SIMD_H
#define SERPENT_SIMD_H

#if defined(__wasm_simd128__)
#include <wasm_simd128.h>
#include <stdint.h>

void serpent_encrypt_4x(const uint32_t *sk, v128_t *W);
#endif

#endif /* SERPENT_SIMD_H */
