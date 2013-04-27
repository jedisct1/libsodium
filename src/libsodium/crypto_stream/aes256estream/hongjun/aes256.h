/* aes256.h */
/* Hongjun Wu, January 2007*/


#include "ecrypt-sync.h"
#include "aes-table.h"

#include <stdio.h>

#define first_round(ctx,x0,y0) { \
        u32 z0,t0,tem0;              \
        z0 = (x0) ^ ctx->round_key[0][0];        \
        t0 = (u8) z0;                \
        tem0 = T0[t0];               \
        (y0) = tem0 ^ ctx->first_round_output_x0; \
}

#define second_round(ctx,x0,y0,y1,y2,y3) { \
        u32 t0,t7,t10,t13; \
        u32 tem0,tem7,tem10,tem13;    \
        t0 = (u8)(x0);      \
        tem0 = T0[t0];      \
        (y0) = tem0 ^ ctx->second_round_output[0];   \
        t7 = (u8)((x0)>>24);     \
        tem7 = T3[t7];    \
        (y1) = tem7 ^ ctx->second_round_output[1];   \
        t10 = (u8)((x0)>>16);   \
        tem10 = T2[t10]; \
        (y2) = tem10 ^ ctx->second_round_output[2];  \
        t13 = (u8)((x0)>>8);    \
        tem13 = T1[t13];\
        (y3) = tem13 ^ ctx->second_round_output[3];  \
}

#define round(ctx,x0,x1,x2,x3,y0,y1,y2,y3,r) { \
        u32 t0,t1,t2,t3;    \
        u32 t4,t5,t6,t7;    \
        u32 t8,t9,t10,t11;  \
        u32 t12,t13,t14,t15;\
        u32 tem0,tem1,tem2,tem3;    \
        u32 tem4,tem5,tem6,tem7;    \
        u32 tem8,tem9,tem10,tem11;  \
        u32 tem12,tem13,tem14,tem15;\
        \
        t0 = (u8)(x0);      \
        tem0 = T0[t0];      \
        t1 = (u8)((x1)>>8); \
        tem1 = tem0 ^ T1[t1];    \
        t2 = (u8)((x2)>>16);     \
        tem2 = tem1 ^ T2[t2];    \
        t3 = (u8)((x3)>>24);     \
        tem3 = tem2 ^ T3[t3];    \
        (y0) = tem3 ^ ctx->round_key[r][0];   \
        \
        t4 = (u8)(x1);      \
        tem4 = T0[t4];      \
        t5 = (u8)((x2)>>8); \
        tem5 = tem4 ^ T1[t5];    \
        t6 = (u8)((x3)>>16);     \
        tem6 = tem5 ^ T2[t6];    \
        t7 = (u8)((x0)>>24);     \
        tem7 = tem6 ^ T3[t7];    \
        (y1) = tem7 ^ ctx->round_key[r][1];   \
        \
        t8 = (u8)(x2);          \
        tem8 = T0[t8];          \
        t9 = (u8)((x3)>>8);     \
        tem9 = tem8 ^ T1[t9];   \
        t10 = (u8)((x0)>>16);   \
        tem10 = tem9 ^ T2[t10]; \
        t11 = (u8)((x1)>>24);   \
        tem11 = tem10 ^ T3[t11];\
        (y2) = tem11 ^ ctx->round_key[r][2]; \
        \
        t12 = (u8)(x3);         \
        tem12 = T0[t12];        \
        t13 = (u8)((x0)>>8);    \
        tem13 = tem12 ^ T1[t13];\
        t14 = (u8)((x1)>>16);   \
        tem14 = tem13 ^ T2[t14];\
        t15 = (u8)((x2)>>24);   \
        tem15 = tem14 ^ T3[t15];\
        (y3) = tem15 ^ ctx->round_key[r][3]; \
}

/* 22.14 cycles/byte*/
#define last_round(ctx,x0,x1,x2,x3,output,r) { \
        u32 t0,t1,t2,t3;    \
        u32 t4,t5,t6,t7;    \
        u32 t8,t9,t10,t11;  \
        u32 t12,t13,t14,t15;\
        \
        t0 = (u8)(x0);        \
        output[0] = Sbox[t0]; \
      t7 = (u8)((x0)>>24);  \
        output[7] = Sbox[t7]; \
        t10 = (u8)((x0)>>16);   \
        output[10] = Sbox[t10]; \
        t13 = (u8)((x0)>>8);    \
        output[13] = Sbox[t13]; \
        \
        t1 = (u8)((x1)>>8);   \
        output[1] = Sbox[t1]; \
        t4 = (u8)(x1);        \
        output[4] = Sbox[t4]; \
        t11 = (u8)((x1)>>24);     \
        output[11] = Sbox[t11]; \
        t14 = (u8)((x1)>>16);     \
        output[14] = Sbox[t14]; \
        \
        t2 = (u8)((x2)>>16);  \
        output[2] = Sbox[t2]; \
        t5 = (u8)((x2)>>8);   \
        output[5] = Sbox[t5]; \
        t8 = (u8)(x2);            \
        output[8] = Sbox[t8]; \
        t15 = (u8)((x2)>>24);     \
        output[15] = Sbox[t15]; \
      \
        t3 = (u8)((x3)>>24);  \
        output[3] = Sbox[t3]; \
        t6 = (u8)((x3)>>16);  \
        output[6] = Sbox[t6]; \
        t9 = (u8)((x3)>>8);       \
        output[9] = Sbox[t9];     \
        t12 = (u8)(x3);           \
        output[12] = Sbox[t12];   \
}

/*
        ((u32*)output)[0] ^= ctx->round_key[r][0]; \
        ((u32*)output)[1] ^= ctx->round_key[r][1]; \
        ((u32*)output)[2] ^= ctx->round_key[r][2]; \
        ((u32*)output)[3] ^= ctx->round_key[r][3]; \
}
*/

#define aes256_enc_block(x,output,ctx) {\
        u32 y0;\
        u32 z0,z1,z2,z3;\
        u32 a0,a1,a2,a3;\
        u32 b0,b1,b2,b3;\
      u32 c0,c1,c2,c3;\
      u32 d0,d1,d2,d3;\
      u32 e0,e1,e2,e3;\
      u32 f0,f1,f2,f3;\
      u32 g0,g1,g2,g3;\
      u32 h0,h1,h2,h3;\
      u32 i0,i1,i2,i3;\
      u32 j0,j1,j2,j3;\
      u32 k0,k1,k2,k3;\
        first_round(ctx,x[0],y0);\
        second_round(ctx,y0,z0,z1,z2,z3);\
        round(ctx,z0,z1,z2,z3,a0,a1,a2,a3,3);\
        round(ctx,a0,a1,a2,a3,b0,b1,b2,b3,4);\
        round(ctx,b0,b1,b2,b3,c0,c1,c2,c3,5);\
        round(ctx,c0,c1,c2,c3,d0,d1,d2,d3,6);\
        round(ctx,d0,d1,d2,d3,e0,e1,e2,e3,7);\
        round(ctx,e0,e1,e2,e3,f0,f1,f2,f3,8);\
        round(ctx,f0,f1,f2,f3,g0,g1,g2,g3,9);\
      round(ctx,g0,g1,g2,g3,h0,h1,h2,h3,10);\
      round(ctx,h0,h1,h2,h3,i0,i1,i2,i3,11);\
      round(ctx,i0,i1,i2,i3,j0,j1,j2,j3,12);\
      round(ctx,j0,j1,j2,j3,k0,k1,k2,k3,13);\
      last_round(ctx,k0,k1,k2,k3,(output),14);\
}

/*compute the intermediate values for the first two rounds*/
void partial_precompute_tworounds(ECRYPT_ctx* ctx)
{
  u32 x0,x1,x2,x3,y0,y1,y2,y3;

  x0 = ctx->counter[0] ^ ctx->round_key[0][0];
  x1 = ctx->counter[1] ^ ctx->round_key[0][1];
  x2 = ctx->counter[2] ^ ctx->round_key[0][2];
  x3 = ctx->counter[3] ^ ctx->round_key[0][3];
  x0 &= 0xffffff00;
  round(ctx,x0,x1,x2,x3,y0,y1,y2,y3,1);
  ctx->first_round_output_x0 = y0 ^ T0[0];
  y0 = 0;
  round(ctx,y0,y1,y2,y3,x0,x1,x2,x3,2);
  ctx->second_round_output[0] = x0 ^ T0[0];
  ctx->second_round_output[1] = x1 ^ T3[0];
  ctx->second_round_output[2] = x2 ^ T2[0];
  ctx->second_round_output[3] = x3 ^ T1[0];
}

