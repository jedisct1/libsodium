/*
 * crypto_box/try.c version 20090118
 * D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "crypto_box.h"
#include "utils.h"
#include "windows/windows-quirks.h"

extern unsigned char *alignedcalloc(unsigned long long);

const char *primitiveimplementation = crypto_box_IMPLEMENTATION;

#define MAXTEST_BYTES 10000
#define CHECKSUM_BYTES 4096
#define TUNE_BYTES 1536

static unsigned char *ska;
static unsigned char *pka;
static unsigned char *skb;
static unsigned char *pkb;
static unsigned char *s;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;
static unsigned char *t;
static unsigned char *ska2;
static unsigned char *pka2;
static unsigned char *skb2;
static unsigned char *pkb2;
static unsigned char *s2;
static unsigned char *n2;
static unsigned char *m2;
static unsigned char *c2;
static unsigned char *t2;

#define sklen crypto_box_SECRETKEYBYTES
#define pklen crypto_box_PUBLICKEYBYTES
#define nlen crypto_box_NONCEBYTES
#define slen crypto_box_BEFORENMBYTES

void preallocate(void)
{
}

void allocate(void)
{
  ska = alignedcalloc(sklen);
  pka = alignedcalloc(pklen);
  skb = alignedcalloc(sklen);
  pkb = alignedcalloc(pklen);
  n = alignedcalloc(nlen);
  m = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  c = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  t = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  s = alignedcalloc(slen);
  ska2 = alignedcalloc(sklen);
  pka2 = alignedcalloc(pklen);
  skb2 = alignedcalloc(sklen);
  pkb2 = alignedcalloc(pklen);
  n2 = alignedcalloc(nlen);
  m2 = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  c2 = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  t2 = alignedcalloc(MAXTEST_BYTES + crypto_box_ZEROBYTES);
  s2 = alignedcalloc(slen);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_box(c,m,TUNE_BYTES + crypto_box_ZEROBYTES,n,pka,skb);
  crypto_box_open(t,c,TUNE_BYTES + crypto_box_ZEROBYTES,n,pkb,ska);
}

char checksum[nlen * 2 + 1];

const char *checksum_compute(void)
{
  long long i;
  long long j;

  if (crypto_box_keypair(pka,ska) != 0) return "crypto_box_keypair returns nonzero";
  if (crypto_box_keypair(pkb,skb) != 0) return "crypto_box_keypair returns nonzero";

  for (j = 0;j < crypto_box_ZEROBYTES;++j) m[j] = 0;

  for (i = 0;i < CHECKSUM_BYTES;++i) {
    long long mlen = i + crypto_box_ZEROBYTES;
    long long tlen = i + crypto_box_ZEROBYTES;
    long long clen = i + crypto_box_ZEROBYTES;

    for (j = -16;j < 0;++j) ska[j] = rand();
    for (j = -16;j < 0;++j) skb[j] = rand();
    for (j = -16;j < 0;++j) pka[j] = rand();
    for (j = -16;j < 0;++j) pkb[j] = rand();
    for (j = -16;j < 0;++j) m[j] = rand();
    for (j = -16;j < 0;++j) n[j] = rand();

    for (j = sklen;j < sklen + 16;++j) ska[j] = rand();
    for (j = sklen;j < sklen + 16;++j) skb[j] = rand();
    for (j = pklen;j < pklen + 16;++j) pka[j] = rand();
    for (j = pklen;j < pklen + 16;++j) pkb[j] = rand();
    for (j = mlen;j < mlen + 16;++j) m[j] = rand();
    for (j = nlen;j < nlen + 16;++j) n[j] = rand();

    for (j = -16;j < sklen + 16;++j) ska2[j] = ska[j];
    for (j = -16;j < sklen + 16;++j) skb2[j] = skb[j];
    for (j = -16;j < pklen + 16;++j) pka2[j] = pka[j];
    for (j = -16;j < pklen + 16;++j) pkb2[j] = pkb[j];
    for (j = -16;j < mlen + 16;++j) m2[j] = m[j];
    for (j = -16;j < nlen + 16;++j) n2[j] = n[j];
    for (j = -16;j < clen + 16;++j) c2[j] = c[j] = rand();

    if (crypto_box(c,m,mlen,n,pkb,ska) != 0) return "crypto_box returns nonzero";

    for (j = -16;j < mlen + 16;++j) if (m2[j] != m[j]) return "crypto_box overwrites m";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_box overwrites n";
    for (j = -16;j < 0;++j) if (c2[j] != c[j]) return "crypto_box writes before output";
    for (j = clen;j < clen + 16;++j) if (c2[j] != c[j]) return "crypto_box writes after output";
    for (j = 0;j < crypto_box_BOXZEROBYTES;++j)
      if (c[j] != 0) return "crypto_box does not clear extra bytes";

    for (j = -16;j < sklen + 16;++j) if (ska2[j] != ska[j]) return "crypto_box overwrites ska";
    for (j = -16;j < sklen + 16;++j) if (skb2[j] != skb[j]) return "crypto_box overwrites skb";
    for (j = -16;j < pklen + 16;++j) if (pka2[j] != pka[j]) return "crypto_box overwrites pka";
    for (j = -16;j < pklen + 16;++j) if (pkb2[j] != pkb[j]) return "crypto_box overwrites pkb";

    for (j = -16;j < 0;++j) c[j] = rand();
    for (j = clen;j < clen + 16;++j) c[j] = rand();
    for (j = -16;j < clen + 16;++j) c2[j] = c[j];
    for (j = -16;j < tlen + 16;++j) t2[j] = t[j] = rand();

    if (crypto_box_open(t,c,clen,n,pka,skb) != 0) return "crypto_box_open returns nonzero";

    for (j = -16;j < clen + 16;++j) if (c2[j] != c[j]) return "crypto_box_open overwrites c";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_box_open overwrites n";
    for (j = -16;j < 0;++j) if (t2[j] != t[j]) return "crypto_box_open writes before output";
    for (j = tlen;j < tlen + 16;++j) if (t2[j] != t[j]) return "crypto_box_open writes after output";
    for (j = 0;j < crypto_box_ZEROBYTES;++j)
      if (t[j] != 0) return "crypto_box_open does not clear extra bytes";

    for (j = -16;j < sklen + 16;++j) if (ska2[j] != ska[j]) return "crypto_box_open overwrites ska";
    for (j = -16;j < sklen + 16;++j) if (skb2[j] != skb[j]) return "crypto_box_open overwrites skb";
    for (j = -16;j < pklen + 16;++j) if (pka2[j] != pka[j]) return "crypto_box_open overwrites pka";
    for (j = -16;j < pklen + 16;++j) if (pkb2[j] != pkb[j]) return "crypto_box_open overwrites pkb";

    for (j = 0;j < mlen;++j) if (t[j] != m[j]) return "plaintext does not match";

    for (j = -16;j < slen + 16;++j) s2[j] = s[j] = rand();
    if (crypto_box_beforenm(s,pkb,ska) != 0) return "crypto_box_beforenm returns nonzero";
    for (j = -16;j < pklen + 16;++j) if (pka2[j] != pka[j]) return "crypto_box_open overwrites pk";
    for (j = -16;j < sklen + 16;++j) if (skb2[j] != skb[j]) return "crypto_box_open overwrites sk";
    for (j = -16;j < 0;++j) if (s2[j] != s[j]) return "crypto_box_beforenm writes before output";
    for (j = slen;j < slen + 16;++j) if (s2[j] != s[j]) return "crypto_box_beforenm writes after output";

    for (j = -16;j < slen + 16;++j) s2[j] = s[j];
    for (j = -16;j < tlen + 16;++j) t2[j] = t[j] = rand();
    if (crypto_box_afternm(t,m,mlen,n,s) != 0) return "crypto_box_afternm returns nonzero";
    for (j = -16;j < slen + 16;++j) if (s2[j] != s[j]) return "crypto_box_afternm overwrites s";
    for (j = -16;j < mlen + 16;++j) if (m2[j] != m[j]) return "crypto_box_afternm overwrites m";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_box_afternm overwrites n";
    for (j = -16;j < 0;++j) if (t2[j] != t[j]) return "crypto_box_afternm writes before output";
    for (j = tlen;j < tlen + 16;++j) if (t2[j] != t[j]) return "crypto_box_afternm writes after output";
    for (j = 0;j < crypto_box_BOXZEROBYTES;++j)
      if (t[j] != 0) return "crypto_box_afternm does not clear extra bytes";
    for (j = 0;j < mlen;++j) if (t[j] != c[j]) return "crypto_box_afternm does not match crypto_box";

    if (crypto_box_beforenm(s,pka,skb) != 0) return "crypto_box_beforenm returns nonzero";

    for (j = -16;j < tlen + 16;++j) t2[j] = t[j] = rand();
    if (crypto_box_open_afternm(t,c,clen,n,s) != 0) return "crypto_box_open_afternm returns nonzero";
    for (j = -16;j < slen + 16;++j) if (s2[j] != s[j]) return "crypto_box_open_afternm overwrites s";
    for (j = -16;j < mlen + 16;++j) if (m2[j] != m[j]) return "crypto_box_open_afternm overwrites m";
    for (j = -16;j < nlen + 16;++j) if (n2[j] != n[j]) return "crypto_box_open_afternm overwrites n";
    for (j = -16;j < 0;++j) if (t2[j] != t[j]) return "crypto_box_open_afternm writes before output";
    for (j = tlen;j < tlen + 16;++j) if (t2[j] != t[j]) return "crypto_box_open_afternm writes after output";
    for (j = 0;j < crypto_box_ZEROBYTES;++j)
      if (t[j] != 0) return "crypto_box_open_afternm does not clear extra bytes";
    for (j = 0;j < mlen;++j) if (t[j] != m[j]) return "crypto_box_open_afternm does not match crypto_box_open";

    for (j = 0;j < i;++j) n[j % nlen] ^= c[j + crypto_box_BOXZEROBYTES];
    if (i == 0) m[crypto_box_ZEROBYTES] = 0;
    m[i + crypto_box_ZEROBYTES] = m[crypto_box_ZEROBYTES];
    for (j = 0;j < i;++j) m[j + crypto_box_ZEROBYTES] ^= c[j + crypto_box_BOXZEROBYTES];
  }

  sodium_bin2hex(checksum, sizeof checksum, n, nlen);

  return 0;
}
