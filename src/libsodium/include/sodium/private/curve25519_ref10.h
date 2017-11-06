#ifndef curve25519_ref10_H
#define curve25519_ref10_H

#include <stddef.h>
#include <stdint.h>

/*
 fe means field element.
 Here the field is \Z/(2^255-19).
 */

#define fe fe25519

#ifdef HAVE_TI_MODE
typedef uint64_t fe[5];
#else
typedef int32_t fe[10];
#endif

void fe_frombytes(fe,const unsigned char *);
void fe_tobytes(unsigned char *,const fe);

void fe_copy(fe,const fe);
int  fe_iszero(const fe);
void fe_0(fe);
void fe_1(fe);
void fe_add(fe,const fe,const fe);
void fe_sub(fe,const fe,const fe);
void fe_mul(fe,const fe,const fe);
void fe_sq(fe,const fe);
void fe_invert(fe,const fe);
void fe_cswap(fe f, fe g, unsigned int b);
void fe_scalar_product(fe h, const fe f, uint32_t n);

/*
 ge means group element.
 *
 Here the group is the set of pairs (x,y) of field elements (see fe.h)
 satisfying -x^2 + y^2 = 1 + d x^2y^2
 where d = -121665/121666.
 *
 Representations:
 ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
 ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
 ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
 ge_precomp (Duif): (y+x,y-x,2dxy)
 */

#define ge_p2 ge25519_p2
typedef struct {
    fe X;
    fe Y;
    fe Z;
} ge_p2;

#define ge_p3 ge25519_p3
typedef struct {
    fe X;
    fe Y;
    fe Z;
    fe T;
} ge_p3;

#define ge_p1p1 ge25519_p1p1
typedef struct {
    fe X;
    fe Y;
    fe Z;
    fe T;
} ge_p1p1;

#define ge_precomp ge25519_precomp
typedef struct {
    fe yplusx;
    fe yminusx;
    fe xy2d;
} ge_precomp;

#define ge_cached ge25519_cached
typedef struct {
    fe YplusX;
    fe YminusX;
    fe Z;
    fe T2d;
} ge_cached;

void ge_tobytes(unsigned char *,const ge_p2 *);
void ge_p3_tobytes(unsigned char *,const ge_p3 *);
int ge_frombytes(ge_p3 *,const unsigned char *);
int ge_frombytes_negate_vartime(ge_p3 *,const unsigned char *);

void ge_p3_to_cached(ge_cached *,const ge_p3 *);
void ge_p1p1_to_p2(ge_p2 *,const ge_p1p1 *);

void ge_add(ge_p1p1 *,const ge_p3 *,const ge_cached *);
void ge_scalarmult_base(ge_p3 *,const unsigned char *);
void ge_double_scalarmult_vartime(ge_p2 *,const unsigned char *,const ge_p3 *,const unsigned char *);
void ge_scalarmult(ge_p3 *,const unsigned char *,const ge_p3 *);
void ge_scalarmult_vartime(ge_p3 *,const unsigned char *,const ge_p3 *);
int ge_is_canonical(const unsigned char *s);
int ge_is_on_curve(const ge_p3 *p);
int ge_is_on_main_subgroup(const ge_p3 *p);
int ge_has_small_order(const unsigned char s[32]);

/*
 The set of scalars is \Z/l
 where l = 2^252 + 27742317777372353535851937790883648493.
 */

void sc_reduce(unsigned char *);
void sc_muladd(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);
int sc_is_canonical(const unsigned char *s);

#endif
