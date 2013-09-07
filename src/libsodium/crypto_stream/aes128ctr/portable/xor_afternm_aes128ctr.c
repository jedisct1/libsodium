/* Author: Peter Schwabe, ported from an assembly implementation by Emilia Käsper
 * Date: 2009-03-19
 * Public domain */

#include <stdio.h>
#include "api.h"
#include "int128.h"
#include "common.h"
#include "consts.h"

int crypto_stream_xor_afternm(unsigned char *outp, const unsigned char *inp, unsigned long long len, const unsigned char *noncep, const unsigned char *c)
{

  int128 xmm0;
  int128 xmm1;
  int128 xmm2;
  int128 xmm3;
  int128 xmm4;
  int128 xmm5;
  int128 xmm6;
  int128 xmm7;

  int128 xmm8;
  int128 xmm9;
  int128 xmm10;
  int128 xmm11;
  int128 xmm12;
  int128 xmm13;
  int128 xmm14;
  int128 xmm15;

  int128 nonce_stack;
  unsigned long long lensav;
  unsigned char bl[128];
  unsigned char *blp;
  unsigned char b;

  uint32 tmp;

  /* Copy nonce on the stack */
  copy2(&nonce_stack, (const int128 *) (noncep + 0));
  unsigned char *np = (unsigned char *)&nonce_stack;

    enc_block:

    xmm0 = *(int128 *) (np + 0);
    copy2(&xmm1, &xmm0);
    shufb(&xmm1, SWAP32);
    copy2(&xmm2, &xmm1);
    copy2(&xmm3, &xmm1);
    copy2(&xmm4, &xmm1);
    copy2(&xmm5, &xmm1);
    copy2(&xmm6, &xmm1);
    copy2(&xmm7, &xmm1);

    add_uint32_big(&xmm1, 1);
    add_uint32_big(&xmm2, 2);
    add_uint32_big(&xmm3, 3);
    add_uint32_big(&xmm4, 4);
    add_uint32_big(&xmm5, 5);
    add_uint32_big(&xmm6, 6);
    add_uint32_big(&xmm7, 7);

    shufb(&xmm0, M0);
    shufb(&xmm1, M0SWAP);
    shufb(&xmm2, M0SWAP);
    shufb(&xmm3, M0SWAP);
    shufb(&xmm4, M0SWAP);
    shufb(&xmm5, M0SWAP);
    shufb(&xmm6, M0SWAP);
    shufb(&xmm7, M0SWAP);

    bitslice(xmm7, xmm6, xmm5, xmm4, xmm3, xmm2, xmm1, xmm0, xmm8)

    aesround( 1, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,c)
    aesround( 2, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,c)
    aesround( 3, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,c)
    aesround( 4, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,c)
    aesround( 5, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,c)
    aesround( 6, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,c)
    aesround( 7, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,c)
    aesround( 8, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,c)
    aesround( 9, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,c)
    lastround(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,c)

    bitslice(xmm13, xmm10, xmm15, xmm11, xmm14, xmm12, xmm9, xmm8, xmm0)

    if(len < 128) goto partial;
    if(len == 128) goto full;

    tmp = load32_bigendian(np + 12);
    tmp += 8;
    store32_bigendian(np + 12, tmp);

    xor2(&xmm8, (const int128 *)(inp + 0));
    xor2(&xmm9, (const int128 *)(inp + 16));
    xor2(&xmm12, (const int128 *)(inp + 32));
    xor2(&xmm14, (const int128 *)(inp + 48));
    xor2(&xmm11, (const int128 *)(inp + 64));
    xor2(&xmm15, (const int128 *)(inp + 80));
    xor2(&xmm10, (const int128 *)(inp + 96));
    xor2(&xmm13, (const int128 *)(inp + 112));

    *(int128 *) (outp + 0) = xmm8;
    *(int128 *) (outp + 16) = xmm9;
    *(int128 *) (outp + 32) = xmm12;
    *(int128 *) (outp + 48) = xmm14;
    *(int128 *) (outp + 64) = xmm11;
    *(int128 *) (outp + 80) = xmm15;
    *(int128 *) (outp + 96) = xmm10;
    *(int128 *) (outp + 112) = xmm13;

    len -= 128;
    inp += 128;
    outp += 128;

    goto enc_block;

    partial:

    lensav = len;
    len >>= 4;

    tmp = load32_bigendian(np + 12);
    tmp += len;
    store32_bigendian(np + 12, tmp);

    blp = bl;
    *(int128 *)(blp + 0) = xmm8;
    *(int128 *)(blp + 16) = xmm9;
    *(int128 *)(blp + 32) = xmm12;
    *(int128 *)(blp + 48) = xmm14;
    *(int128 *)(blp + 64) = xmm11;
    *(int128 *)(blp + 80) = xmm15;
    *(int128 *)(blp + 96) = xmm10;
    *(int128 *)(blp + 112) = xmm13;

    bytes:

    if(lensav == 0) goto end;

    b = blp[0]; /* clang false positive */
    b ^= *(const unsigned char *)(inp + 0);
    *(unsigned char *)(outp + 0) = b;

    blp += 1;
    inp +=1;
    outp +=1;
    lensav -= 1;

    goto bytes;

    full:

    tmp = load32_bigendian(np + 12);
    tmp += 8;
    store32_bigendian(np + 12, tmp);

    xor2(&xmm8, (const int128 *)(inp + 0));
    xor2(&xmm9, (const int128 *)(inp + 16));
    xor2(&xmm12, (const int128 *)(inp + 32));
    xor2(&xmm14, (const int128 *)(inp + 48));
    xor2(&xmm11, (const int128 *)(inp + 64));
    xor2(&xmm15, (const int128 *)(inp + 80));
    xor2(&xmm10, (const int128 *)(inp + 96));
    xor2(&xmm13, (const int128 *)(inp + 112));

    *(int128 *) (outp + 0) = xmm8;
    *(int128 *) (outp + 16) = xmm9;
    *(int128 *) (outp + 32) = xmm12;
    *(int128 *) (outp + 48) = xmm14;
    *(int128 *) (outp + 64) = xmm11;
    *(int128 *) (outp + 80) = xmm15;
    *(int128 *) (outp + 96) = xmm10;
    *(int128 *) (outp + 112) = xmm13;

    end:
    return 0;

}
