/*
Copyright (c) 2018 Algorand LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <string.h>

#include "crypto_hash_sha512.h"
#include "crypto_vrf_ietfdraft03.h"
#include "private/ed25519_ref10.h"
#include "vrf_ietfdraft03.h"

static const unsigned char ONE = 0x01;
static const unsigned char TWO = 0x02;

/* Encode elliptic curve point into a 32-byte octet string per RFC8032 section
 * 5.1.2.
 */
void
_vrf_ietfdraft03_point_to_string(unsigned char string[32], const ge25519_p3 *point)
{
    ge25519_p3_tobytes(string, point);
}

/* Decode elliptic curve point from 32-byte octet string per RFC8032 section
 * 5.1.3.
 *
 * In particular we must reject non-canonical encodings (i.e., when the encoded
 * y coordinate is not reduced mod p). We do not check whether the point is on
 * the main subgroup or whether it is of low order. Returns 0 on success (and
 * stores decoded point in *point), nonzero if decoding fails.
 */
int
_vrf_ietfdraft03_string_to_point(ge25519_p3 *point, const unsigned char string[32])
{
    if (ge25519_is_canonical(string) == 0 ||
	ge25519_frombytes(point, string) != 0) {
	return -1;
    }
    return 0;
}

/* Hash a message to a curve point using Elligator2.
 * Specified in VRF draft spec section 5.4.1.2.
 * The actual elligator2 implementation is ge25519_from_uniform.
 * Runtime depends only on alphalen (the message length)
 */
void
_vrf_ietfdraft03_hash_to_curve_elligator2_25519(unsigned char H_string[32],
						const ge25519_p3 *Y_point,
						const unsigned char *alpha,
						const unsigned long long alphalen)
{
    crypto_hash_sha512_state hs;
    unsigned char            Y_string[32], r_string[64];

    _vrf_ietfdraft03_point_to_string(Y_string, Y_point);

    /* r = first 32 bytes of SHA512(suite || 0x01 || Y || alpha) */
    crypto_hash_sha512_init(&hs);
    crypto_hash_sha512_update(&hs, &SUITE, 1);
    crypto_hash_sha512_update(&hs, &ONE, 1);
    crypto_hash_sha512_update(&hs, Y_string, 32);
    crypto_hash_sha512_update(&hs, alpha, alphalen);
    crypto_hash_sha512_final(&hs, r_string);

    r_string[31] &= 0x7f; /* clear sign bit */
    ge25519_from_uniform(H_string, r_string); /* elligator2 */
}

/* Subroutine specified in draft spec section 5.4.3.
 * Hashes four points to a 16-byte string.
 * Constant time. */
void
_vrf_ietfdraft03_hash_points(unsigned char c[16], const ge25519_p3 *P1,
			     const ge25519_p3 *P2, const ge25519_p3 *P3,
			     const ge25519_p3 *P4)
{
    unsigned char str[2+32*4], c1[64];

    str[0] = SUITE;
    str[1] = TWO;
    _vrf_ietfdraft03_point_to_string(str+2+32*0, P1);
    _vrf_ietfdraft03_point_to_string(str+2+32*1, P2);
    _vrf_ietfdraft03_point_to_string(str+2+32*2, P3);
    _vrf_ietfdraft03_point_to_string(str+2+32*3, P4);
    crypto_hash_sha512(c1, str, sizeof str);
    memmove(c, c1, 16);
    sodium_memzero(c1, 64);
}

/* Decode an 80-byte proof pi into a point gamma, a 16-byte scalar c, and a
 * 32-byte scalar s, as specified in IETF draft section 5.4.4.
 * Returns 0 on success, nonzero on failure.
 */
int
_vrf_ietfdraft03_decode_proof(ge25519_p3 *Gamma, unsigned char c[16],
			      unsigned char s[32], const unsigned char pi[80])
{
    /* gamma = decode_point(pi[0:32]) */
    if (_vrf_ietfdraft03_string_to_point(Gamma, pi) != 0) {
	return -1;
    }
    memmove(c, pi+32, 16); /* c = pi[32:48] */
    memmove(s, pi+48, 32); /* s = pi[48:80] */
    return 0;
}
