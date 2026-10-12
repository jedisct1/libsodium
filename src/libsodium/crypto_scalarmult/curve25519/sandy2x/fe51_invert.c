/* Use bytes to support either ref10 field representation. */

#include "fe51.h"

#ifdef HAVE_AVX_ASM

#include "private/common.h"
#include "private/ed25519_ref10.h"

void
fe51_invert(fe51 *r, const fe51 *x)
{
    const uint64_t mask = 0x7ffffffffffffULL;
    unsigned char  s[32];
    fe25519        t;

    fe51_pack(s, x);
    fe25519_frombytes(t, s);
    fe25519_invert(t, t);
    fe25519_tobytes(s, t);

    r->v[0] = (LOAD64_LE(s     )      ) & mask;
    r->v[1] = (LOAD64_LE(s +  6) >>  3) & mask;
    r->v[2] = (LOAD64_LE(s + 12) >>  6) & mask;
    r->v[3] = (LOAD64_LE(s + 19) >>  1) & mask;
    r->v[4] = (LOAD64_LE(s + 24) >> 12) & mask;
}

#endif
