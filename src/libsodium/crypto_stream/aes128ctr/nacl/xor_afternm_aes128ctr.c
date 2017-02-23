/* Author: Peter Schwabe, ported from an assembly implementation by Emilia
 * Käsper
 * Date: 2009-03-19
 * Public domain */

#include "common.h"
#include "consts.h"
#include "crypto_stream_aes128ctr.h"
#include "int128.h"

int
crypto_stream_aes128ctr_xor_afternm(unsigned char *out, const unsigned char *in,
                                    unsigned long long   len,
                                    const unsigned char *nonce,
                                    const unsigned char *c)
{
    aes_uint128_t      xmm0;
    aes_uint128_t      xmm1;
    aes_uint128_t      xmm2;
    aes_uint128_t      xmm3;
    aes_uint128_t      xmm4;
    aes_uint128_t      xmm5;
    aes_uint128_t      xmm6;
    aes_uint128_t      xmm7;
    aes_uint128_t      xmm8;
    aes_uint128_t      xmm9;
    aes_uint128_t      xmm10;
    aes_uint128_t      xmm11;
    aes_uint128_t      xmm12;
    aes_uint128_t      xmm13;
    aes_uint128_t      xmm14;
    aes_uint128_t      xmm15;
    aes_uint128_t      nonce_stack;
    unsigned long long lensav;
    unsigned char      bl[128];
    unsigned char *    blp;
    unsigned char *    np;
    unsigned char      b;
    uint32_t           tmp;

    /* Copy nonce on the stack */
    copy2(&nonce_stack, (const aes_uint128_t *) (nonce + 0));
    np = (unsigned char *) &nonce_stack;

enc_block:

    xmm0 = *(aes_uint128_t *) (np + 0);
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

    bitslice(xmm7, xmm6, xmm5, xmm4, xmm3, xmm2, xmm1, xmm0, xmm8);

    aesround(1, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9,
             xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);
    aesround(2, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0,
             xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, c);
    aesround(3, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9,
             xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);
    aesround(4, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0,
             xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, c);
    aesround(5, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9,
             xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);
    aesround(6, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0,
             xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, c);
    aesround(7, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9,
             xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);
    aesround(8, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0,
             xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, c);
    aesround(9, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9,
             xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);
    lastround(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1,
              xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, c);

    bitslice(xmm13, xmm10, xmm15, xmm11, xmm14, xmm12, xmm9, xmm8, xmm0);

    if (len < 128) {
        goto partial;
    }
    if (len == 128) {
        goto full;
    }
    tmp = LOAD32_BE(np + 12);
    tmp += 8;
    STORE32_BE(np + 12, tmp);

    xor2(&xmm8, (const aes_uint128_t *) (in + 0));
    xor2(&xmm9, (const aes_uint128_t *) (in + 16));
    xor2(&xmm12, (const aes_uint128_t *) (in + 32));
    xor2(&xmm14, (const aes_uint128_t *) (in + 48));
    xor2(&xmm11, (const aes_uint128_t *) (in + 64));
    xor2(&xmm15, (const aes_uint128_t *) (in + 80));
    xor2(&xmm10, (const aes_uint128_t *) (in + 96));
    xor2(&xmm13, (const aes_uint128_t *) (in + 112));

    *(aes_uint128_t *) (out + 0)   = xmm8;
    *(aes_uint128_t *) (out + 16)  = xmm9;
    *(aes_uint128_t *) (out + 32)  = xmm12;
    *(aes_uint128_t *) (out + 48)  = xmm14;
    *(aes_uint128_t *) (out + 64)  = xmm11;
    *(aes_uint128_t *) (out + 80)  = xmm15;
    *(aes_uint128_t *) (out + 96)  = xmm10;
    *(aes_uint128_t *) (out + 112) = xmm13;

    len -= 128;
    in += 128;
    out += 128;

    goto enc_block;

partial:

    lensav = len;
    len >>= 4;

    tmp = LOAD32_BE(np + 12);
    tmp += len;
    STORE32_BE(np + 12, tmp);

    blp = bl;

    *(aes_uint128_t *) (blp + 0)   = xmm8;
    *(aes_uint128_t *) (blp + 16)  = xmm9;
    *(aes_uint128_t *) (blp + 32)  = xmm12;
    *(aes_uint128_t *) (blp + 48)  = xmm14;
    *(aes_uint128_t *) (blp + 64)  = xmm11;
    *(aes_uint128_t *) (blp + 80)  = xmm15;
    *(aes_uint128_t *) (blp + 96)  = xmm10;
    *(aes_uint128_t *) (blp + 112) = xmm13;

bytes:

    if (lensav == 0) {
        goto end;
    }
    b = blp[0]; /* clang false positive */
    b ^= *(const unsigned char *) (in + 0);
    *(unsigned char *) (out + 0) = b;

    blp += 1;
    in += 1;
    out += 1;
    lensav -= 1;

    goto bytes;

full:

    tmp = LOAD32_BE(np + 12);
    tmp += 8;
    STORE32_BE(np + 12, tmp);

    xor2(&xmm8, (const aes_uint128_t *) (in + 0));
    xor2(&xmm9, (const aes_uint128_t *) (in + 16));
    xor2(&xmm12, (const aes_uint128_t *) (in + 32));
    xor2(&xmm14, (const aes_uint128_t *) (in + 48));
    xor2(&xmm11, (const aes_uint128_t *) (in + 64));
    xor2(&xmm15, (const aes_uint128_t *) (in + 80));
    xor2(&xmm10, (const aes_uint128_t *) (in + 96));
    xor2(&xmm13, (const aes_uint128_t *) (in + 112));

    *(aes_uint128_t *) (out + 0)   = xmm8;
    *(aes_uint128_t *) (out + 16)  = xmm9;
    *(aes_uint128_t *) (out + 32)  = xmm12;
    *(aes_uint128_t *) (out + 48)  = xmm14;
    *(aes_uint128_t *) (out + 64)  = xmm11;
    *(aes_uint128_t *) (out + 80)  = xmm15;
    *(aes_uint128_t *) (out + 96)  = xmm10;
    *(aes_uint128_t *) (out + 112) = xmm13;

end:
    return 0;
}
