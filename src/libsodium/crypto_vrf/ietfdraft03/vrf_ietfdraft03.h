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

#ifndef vrf_ietfdraft03_H
#define vrf_ietfdraft03_H

static const unsigned char SUITE = 0x04; /* ECVRF-ED25519-SHA512-Elligator2 */

void _vrf_ietfdraft03_point_to_string(unsigned char string[32],
				      const ge25519_p3 *point);

int _vrf_ietfdraft03_string_to_point(ge25519_p3 *point,
				     const unsigned char string[32]);

int _vrf_ietfdraft03_decode_proof(ge25519_p3 *Gamma, unsigned char c[16],
				  unsigned char s[32],
				  const unsigned char pi[80]);

void _vrf_ietfdraft03_hash_to_curve_elligator2_25519(unsigned char H_string[32],
						     const ge25519_p3 *Y_point,
						     const unsigned char *alpha,
						     const unsigned long long alphalen);

void _vrf_ietfdraft03_hash_points(unsigned char c[16], const ge25519_p3 *P1,
				  const ge25519_p3 *P2, const ge25519_p3 *P3,
				  const ge25519_p3 *P4);

#endif
