#include "int128.h"
#include "common.h"

void xor2(int128 *r, const int128 *x)
{
  r->a ^= x->a;
  r->b ^= x->b;
}

void and2(int128 *r, const int128 *x)
{
  r->a &= x->a;
  r->b &= x->b;
}

void or2(int128 *r, const int128 *x)
{
  r->a |= x->a;
  r->b |= x->b;
}

void copy2(int128 *r, const int128 *x)
{
  r->a = x->a;
  r->b = x->b;
}

void shufb(int128 *r, const unsigned char *l)
{
  int128 t;
  copy2(&t,r);
  unsigned char *cr = (unsigned char *)r;
  unsigned char *ct = (unsigned char *)&t;
  cr[0] = ct[l[0]];
  cr[1] = ct[l[1]];
  cr[2] = ct[l[2]];
  cr[3] = ct[l[3]];
  cr[4] = ct[l[4]];
  cr[5] = ct[l[5]];
  cr[6] = ct[l[6]];
  cr[7] = ct[l[7]];
  cr[8] = ct[l[8]];
  cr[9] = ct[l[9]];
  cr[10] = ct[l[10]];
  cr[11] = ct[l[11]];
  cr[12] = ct[l[12]];
  cr[13] = ct[l[13]];
  cr[14] = ct[l[14]];
  cr[15] = ct[l[15]];
}

void shufd(int128 *r, const int128 *x, const unsigned int c)
{
  int128 t;
  uint32 *tp = (uint32 *)&t;
  uint32 *xp = (uint32 *)x;
  tp[0] = xp[c&3];
  tp[1] = xp[(c>>2)&3];
  tp[2] = xp[(c>>4)&3];
  tp[3] = xp[(c>>6)&3];
  copy2(r,&t);
}

void rshift32_littleendian(int128 *r, const unsigned int n)
{
  unsigned char *rp = (unsigned char *)r;
  uint32 t;
  t = load32_littleendian(rp);
  t >>= n;
  store32_littleendian(rp, t);
  t = load32_littleendian(rp+4);
  t >>= n;
  store32_littleendian(rp+4, t);
  t = load32_littleendian(rp+8);
  t >>= n;
  store32_littleendian(rp+8, t);
  t = load32_littleendian(rp+12);
  t >>= n;
  store32_littleendian(rp+12, t);
}

void rshift64_littleendian(int128 *r, const unsigned int n)
{
  unsigned char *rp = (unsigned char *)r;
  uint64 t;
  t = load64_littleendian(rp);
  t >>= n;
  store64_littleendian(rp, t);
  t = load64_littleendian(rp+8);
  t >>= n;
  store64_littleendian(rp+8, t);
}

void lshift64_littleendian(int128 *r, const unsigned int n)
{
  unsigned char *rp = (unsigned char *)r;
  uint64 t;
  t = load64_littleendian(rp);
  t <<= n;
  store64_littleendian(rp, t);
  t = load64_littleendian(rp+8);
  t <<= n;
  store64_littleendian(rp+8, t);
}

void toggle(int128 *r)
{
  r->a ^= 0xffffffffffffffffULL;
  r->b ^= 0xffffffffffffffffULL;
}

void xor_rcon(int128 *r)
{
  unsigned char *rp = (unsigned char *)r;
  uint32 t;
  t = load32_littleendian(rp+12);
  t ^= 0xffffffff;
  store32_littleendian(rp+12, t);
}

void add_uint32_big(int128 *r, uint32 x)
{
  unsigned char *rp = (unsigned char *)r;
  uint32 t;
  t = load32_littleendian(rp+12);
  t += x;
  store32_littleendian(rp+12, t);
}
