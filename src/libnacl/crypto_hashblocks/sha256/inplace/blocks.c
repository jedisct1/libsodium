#include "crypto_hashblocks.h"

typedef unsigned int uint32;

static uint32 load_bigendian(const unsigned char *x)
{
  return
      (uint32) (x[3]) \
  | (((uint32) (x[2])) << 8) \
  | (((uint32) (x[1])) << 16) \
  | (((uint32) (x[0])) << 24)
  ;
}

static void store_bigendian(unsigned char *x,uint32 u)
{
  x[3] = u; u >>= 8;
  x[2] = u; u >>= 8;
  x[1] = u; u >>= 8;
  x[0] = u;
}

#define SHR(x,c) ((x) >> (c))
#define ROTR(x,c) (((x) >> (c)) | ((x) << (32 - (c))))

#define Ch(x,y,z) ((x & y) ^ (~x & z))
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
#define Sigma0(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define Sigma1(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))
#define sigma0(x) (ROTR(x, 7) ^ ROTR(x,18) ^ SHR(x, 3))
#define sigma1(x) (ROTR(x,17) ^ ROTR(x,19) ^ SHR(x,10))

#define M(w0,w14,w9,w1) w0 += sigma1(w14) + w9 + sigma0(w1);

#define EXPAND \
  M(w0 ,w14,w9 ,w1 ) \
  M(w1 ,w15,w10,w2 ) \
  M(w2 ,w0 ,w11,w3 ) \
  M(w3 ,w1 ,w12,w4 ) \
  M(w4 ,w2 ,w13,w5 ) \
  M(w5 ,w3 ,w14,w6 ) \
  M(w6 ,w4 ,w15,w7 ) \
  M(w7 ,w5 ,w0 ,w8 ) \
  M(w8 ,w6 ,w1 ,w9 ) \
  M(w9 ,w7 ,w2 ,w10) \
  M(w10,w8 ,w3 ,w11) \
  M(w11,w9 ,w4 ,w12) \
  M(w12,w10,w5 ,w13) \
  M(w13,w11,w6 ,w14) \
  M(w14,w12,w7 ,w15) \
  M(w15,w13,w8 ,w0 )

#define F(r0,r1,r2,r3,r4,r5,r6,r7,w,k) \
  r7 += Sigma1(r4) + Ch(r4,r5,r6) + k + w; \
  r3 += r7; \
  r7 += Sigma0(r0) + Maj(r0,r1,r2);

#define G(r0,r1,r2,r3,r4,r5,r6,r7,i) \
  F(r0,r1,r2,r3,r4,r5,r6,r7,w0 ,round[i + 0]) \
  F(r7,r0,r1,r2,r3,r4,r5,r6,w1 ,round[i + 1]) \
  F(r6,r7,r0,r1,r2,r3,r4,r5,w2 ,round[i + 2]) \
  F(r5,r6,r7,r0,r1,r2,r3,r4,w3 ,round[i + 3]) \
  F(r4,r5,r6,r7,r0,r1,r2,r3,w4 ,round[i + 4]) \
  F(r3,r4,r5,r6,r7,r0,r1,r2,w5 ,round[i + 5]) \
  F(r2,r3,r4,r5,r6,r7,r0,r1,w6 ,round[i + 6]) \
  F(r1,r2,r3,r4,r5,r6,r7,r0,w7 ,round[i + 7]) \
  F(r0,r1,r2,r3,r4,r5,r6,r7,w8 ,round[i + 8]) \
  F(r7,r0,r1,r2,r3,r4,r5,r6,w9 ,round[i + 9]) \
  F(r6,r7,r0,r1,r2,r3,r4,r5,w10,round[i + 10]) \
  F(r5,r6,r7,r0,r1,r2,r3,r4,w11,round[i + 11]) \
  F(r4,r5,r6,r7,r0,r1,r2,r3,w12,round[i + 12]) \
  F(r3,r4,r5,r6,r7,r0,r1,r2,w13,round[i + 13]) \
  F(r2,r3,r4,r5,r6,r7,r0,r1,w14,round[i + 14]) \
  F(r1,r2,r3,r4,r5,r6,r7,r0,w15,round[i + 15])

static const uint32 round[64] = {
  0x428a2f98
, 0x71374491
, 0xb5c0fbcf
, 0xe9b5dba5
, 0x3956c25b
, 0x59f111f1
, 0x923f82a4
, 0xab1c5ed5
, 0xd807aa98
, 0x12835b01
, 0x243185be
, 0x550c7dc3
, 0x72be5d74
, 0x80deb1fe
, 0x9bdc06a7
, 0xc19bf174
, 0xe49b69c1
, 0xefbe4786
, 0x0fc19dc6
, 0x240ca1cc
, 0x2de92c6f
, 0x4a7484aa
, 0x5cb0a9dc
, 0x76f988da
, 0x983e5152
, 0xa831c66d
, 0xb00327c8
, 0xbf597fc7
, 0xc6e00bf3
, 0xd5a79147
, 0x06ca6351
, 0x14292967
, 0x27b70a85
, 0x2e1b2138
, 0x4d2c6dfc
, 0x53380d13
, 0x650a7354
, 0x766a0abb
, 0x81c2c92e
, 0x92722c85
, 0xa2bfe8a1
, 0xa81a664b
, 0xc24b8b70
, 0xc76c51a3
, 0xd192e819
, 0xd6990624
, 0xf40e3585
, 0x106aa070
, 0x19a4c116
, 0x1e376c08
, 0x2748774c
, 0x34b0bcb5
, 0x391c0cb3
, 0x4ed8aa4a
, 0x5b9cca4f
, 0x682e6ff3
, 0x748f82ee
, 0x78a5636f
, 0x84c87814
, 0x8cc70208
, 0x90befffa
, 0xa4506ceb
, 0xbef9a3f7
, 0xc67178f2
} ;

int crypto_hashblocks(unsigned char *statebytes,const unsigned char *in,unsigned long long inlen)
{
  uint32 state[8];
  uint32 r0;
  uint32 r1;
  uint32 r2;
  uint32 r3;
  uint32 r4;
  uint32 r5;
  uint32 r6;
  uint32 r7;

  r0 = load_bigendian(statebytes +  0); state[0] = r0;
  r1 = load_bigendian(statebytes +  4); state[1] = r1;
  r2 = load_bigendian(statebytes +  8); state[2] = r2;
  r3 = load_bigendian(statebytes + 12); state[3] = r3;
  r4 = load_bigendian(statebytes + 16); state[4] = r4;
  r5 = load_bigendian(statebytes + 20); state[5] = r5;
  r6 = load_bigendian(statebytes + 24); state[6] = r6;
  r7 = load_bigendian(statebytes + 28); state[7] = r7;

  while (inlen >= 64) {
    uint32 w0  = load_bigendian(in +  0);
    uint32 w1  = load_bigendian(in +  4);
    uint32 w2  = load_bigendian(in +  8);
    uint32 w3  = load_bigendian(in + 12);
    uint32 w4  = load_bigendian(in + 16);
    uint32 w5  = load_bigendian(in + 20);
    uint32 w6  = load_bigendian(in + 24);
    uint32 w7  = load_bigendian(in + 28);
    uint32 w8  = load_bigendian(in + 32);
    uint32 w9  = load_bigendian(in + 36);
    uint32 w10 = load_bigendian(in + 40);
    uint32 w11 = load_bigendian(in + 44);
    uint32 w12 = load_bigendian(in + 48);
    uint32 w13 = load_bigendian(in + 52);
    uint32 w14 = load_bigendian(in + 56);
    uint32 w15 = load_bigendian(in + 60);

    G(r0,r1,r2,r3,r4,r5,r6,r7,0)

    EXPAND

    G(r0,r1,r2,r3,r4,r5,r6,r7,16)

    EXPAND

    G(r0,r1,r2,r3,r4,r5,r6,r7,32)

    EXPAND

    G(r0,r1,r2,r3,r4,r5,r6,r7,48)

    r0 += state[0];
    r1 += state[1];
    r2 += state[2];
    r3 += state[3];
    r4 += state[4];
    r5 += state[5];
    r6 += state[6];
    r7 += state[7];

    state[0] = r0;
    state[1] = r1;
    state[2] = r2;
    state[3] = r3;
    state[4] = r4;
    state[5] = r5;
    state[6] = r6;
    state[7] = r7;

    in += 64;
    inlen -= 64;
  }

  store_bigendian(statebytes +  0,state[0]);
  store_bigendian(statebytes +  4,state[1]);
  store_bigendian(statebytes +  8,state[2]);
  store_bigendian(statebytes + 12,state[3]);
  store_bigendian(statebytes + 16,state[4]);
  store_bigendian(statebytes + 20,state[5]);
  store_bigendian(statebytes + 24,state[6]);
  store_bigendian(statebytes + 28,state[7]);

  return 0;
}
