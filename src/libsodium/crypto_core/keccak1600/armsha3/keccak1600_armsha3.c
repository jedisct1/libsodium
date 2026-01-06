#include <stdint.h>
#include <string.h>

#include "keccak1600_armsha3.h"
#include "private/common.h"

#if defined(__ARM_FEATURE_SHA3)

#include <arm_neon.h>

#ifdef __clang__
# pragma clang attribute push(__attribute__((target("neon,sha3"))), apply_to = function)
#elif defined(__GNUC__)
# pragma GCC target("+simd+sha3")
#endif

#define KECCAK1600_STATEBYTES 200

static const uint64_t RC[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
    0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define ROUND(A, E, rc_idx)                                                    \
    do {                                                                       \
        uint64x2_t rc = vdupq_n_u64(RC[rc_idx]);                               \
        Ca = veor3q_u64(A##ba, veor3q_u64(A##ga, A##ka, A##ma), A##sa);        \
        Ce = veor3q_u64(A##be, veor3q_u64(A##ge, A##ke, A##me), A##se);        \
        Ci = veor3q_u64(A##bi, veor3q_u64(A##gi, A##ki, A##mi), A##si);        \
        Co = veor3q_u64(A##bo, veor3q_u64(A##go, A##ko, A##mo), A##so);        \
        Cu = veor3q_u64(A##bu, veor3q_u64(A##gu, A##ku, A##mu), A##su);        \
        Da = vrax1q_u64(Cu, Ce);                                               \
        De = vrax1q_u64(Ca, Ci);                                               \
        Di = vrax1q_u64(Ce, Co);                                               \
        Do = vrax1q_u64(Ci, Cu);                                               \
        Du = vrax1q_u64(Co, Ca);                                               \
        Ba = veorq_u64(A##ba, Da);                                             \
        Be = vxarq_u64(A##ge, De, 64 - 44);                                    \
        Bi = vxarq_u64(A##ki, Di, 64 - 43);                                    \
        Bo = vxarq_u64(A##mo, Do, 64 - 21);                                    \
        Bu = vxarq_u64(A##su, Du, 64 - 14);                                    \
        E##ba = veorq_u64(vbcaxq_u64(Ba, Bi, Be), rc);                         \
        E##be = vbcaxq_u64(Be, Bo, Bi);                                        \
        E##bi = vbcaxq_u64(Bi, Bu, Bo);                                        \
        E##bo = vbcaxq_u64(Bo, Ba, Bu);                                        \
        E##bu = vbcaxq_u64(Bu, Be, Ba);                                        \
        Ba = vxarq_u64(A##bo, Do, 64 - 28);                                    \
        Be = vxarq_u64(A##gu, Du, 64 - 20);                                    \
        Bi = vxarq_u64(A##ka, Da, 64 - 3);                                     \
        Bo = vxarq_u64(A##me, De, 64 - 45);                                    \
        Bu = vxarq_u64(A##si, Di, 64 - 61);                                    \
        E##ga = vbcaxq_u64(Ba, Bi, Be);                                        \
        E##ge = vbcaxq_u64(Be, Bo, Bi);                                        \
        E##gi = vbcaxq_u64(Bi, Bu, Bo);                                        \
        E##go = vbcaxq_u64(Bo, Ba, Bu);                                        \
        E##gu = vbcaxq_u64(Bu, Be, Ba);                                        \
        Ba = vxarq_u64(A##be, De, 64 - 1);                                     \
        Be = vxarq_u64(A##gi, Di, 64 - 6);                                     \
        Bi = vxarq_u64(A##ko, Do, 64 - 25);                                    \
        Bo = vxarq_u64(A##mu, Du, 64 - 8);                                     \
        Bu = vxarq_u64(A##sa, Da, 64 - 18);                                    \
        E##ka = vbcaxq_u64(Ba, Bi, Be);                                        \
        E##ke = vbcaxq_u64(Be, Bo, Bi);                                        \
        E##ki = vbcaxq_u64(Bi, Bu, Bo);                                        \
        E##ko = vbcaxq_u64(Bo, Ba, Bu);                                        \
        E##ku = vbcaxq_u64(Bu, Be, Ba);                                        \
        Ba = vxarq_u64(A##bu, Du, 64 - 27);                                    \
        Be = vxarq_u64(A##ga, Da, 64 - 36);                                    \
        Bi = vxarq_u64(A##ke, De, 64 - 10);                                    \
        Bo = vxarq_u64(A##mi, Di, 64 - 15);                                    \
        Bu = vxarq_u64(A##so, Do, 64 - 56);                                    \
        E##ma = vbcaxq_u64(Ba, Bi, Be);                                        \
        E##me = vbcaxq_u64(Be, Bo, Bi);                                        \
        E##mi = vbcaxq_u64(Bi, Bu, Bo);                                        \
        E##mo = vbcaxq_u64(Bo, Ba, Bu);                                        \
        E##mu = vbcaxq_u64(Bu, Be, Ba);                                        \
        Ba = vxarq_u64(A##bi, Di, 64 - 62);                                    \
        Be = vxarq_u64(A##go, Do, 64 - 55);                                    \
        Bi = vxarq_u64(A##ku, Du, 64 - 39);                                    \
        Bo = vxarq_u64(A##ma, Da, 64 - 41);                                    \
        Bu = vxarq_u64(A##se, De, 64 - 2);                                     \
        E##sa = vbcaxq_u64(Ba, Bi, Be);                                        \
        E##se = vbcaxq_u64(Be, Bo, Bi);                                        \
        E##si = vbcaxq_u64(Bi, Bu, Bo);                                        \
        E##so = vbcaxq_u64(Bo, Ba, Bu);                                        \
        E##su = vbcaxq_u64(Bu, Be, Ba);                                        \
    } while (0)

static void
keccakf_24_rounds(uint64_t st[25])
{
    uint64x2_t Aba, Abe, Abi, Abo, Abu;
    uint64x2_t Aga, Age, Agi, Ago, Agu;
    uint64x2_t Aka, Ake, Aki, Ako, Aku;
    uint64x2_t Ama, Ame, Ami, Amo, Amu;
    uint64x2_t Asa, Ase, Asi, Aso, Asu;
    uint64x2_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64x2_t Ega, Ege, Egi, Ego, Egu;
    uint64x2_t Eka, Eke, Eki, Eko, Eku;
    uint64x2_t Ema, Eme, Emi, Emo, Emu;
    uint64x2_t Esa, Ese, Esi, Eso, Esu;
    uint64x2_t Ca, Ce, Ci, Co, Cu;
    uint64x2_t Da, De, Di, Do, Du;
    uint64x2_t Ba, Be, Bi, Bo, Bu;

    Aba = vdupq_n_u64(st[0]);  Abe = vdupq_n_u64(st[1]);
    Abi = vdupq_n_u64(st[2]);  Abo = vdupq_n_u64(st[3]);
    Abu = vdupq_n_u64(st[4]);
    Aga = vdupq_n_u64(st[5]);  Age = vdupq_n_u64(st[6]);
    Agi = vdupq_n_u64(st[7]);  Ago = vdupq_n_u64(st[8]);
    Agu = vdupq_n_u64(st[9]);
    Aka = vdupq_n_u64(st[10]); Ake = vdupq_n_u64(st[11]);
    Aki = vdupq_n_u64(st[12]); Ako = vdupq_n_u64(st[13]);
    Aku = vdupq_n_u64(st[14]);
    Ama = vdupq_n_u64(st[15]); Ame = vdupq_n_u64(st[16]);
    Ami = vdupq_n_u64(st[17]); Amo = vdupq_n_u64(st[18]);
    Amu = vdupq_n_u64(st[19]);
    Asa = vdupq_n_u64(st[20]); Ase = vdupq_n_u64(st[21]);
    Asi = vdupq_n_u64(st[22]); Aso = vdupq_n_u64(st[23]);
    Asu = vdupq_n_u64(st[24]);

    ROUND(A, E, 0);  ROUND(E, A, 1);
    ROUND(A, E, 2);  ROUND(E, A, 3);
    ROUND(A, E, 4);  ROUND(E, A, 5);
    ROUND(A, E, 6);  ROUND(E, A, 7);
    ROUND(A, E, 8);  ROUND(E, A, 9);
    ROUND(A, E, 10); ROUND(E, A, 11);
    ROUND(A, E, 12); ROUND(E, A, 13);
    ROUND(A, E, 14); ROUND(E, A, 15);
    ROUND(A, E, 16); ROUND(E, A, 17);
    ROUND(A, E, 18); ROUND(E, A, 19);
    ROUND(A, E, 20); ROUND(E, A, 21);
    ROUND(A, E, 22); ROUND(E, A, 23);

    st[0]  = vgetq_lane_u64(Aba, 0); st[1]  = vgetq_lane_u64(Abe, 0);
    st[2]  = vgetq_lane_u64(Abi, 0); st[3]  = vgetq_lane_u64(Abo, 0);
    st[4]  = vgetq_lane_u64(Abu, 0);
    st[5]  = vgetq_lane_u64(Aga, 0); st[6]  = vgetq_lane_u64(Age, 0);
    st[7]  = vgetq_lane_u64(Agi, 0); st[8]  = vgetq_lane_u64(Ago, 0);
    st[9]  = vgetq_lane_u64(Agu, 0);
    st[10] = vgetq_lane_u64(Aka, 0); st[11] = vgetq_lane_u64(Ake, 0);
    st[12] = vgetq_lane_u64(Aki, 0); st[13] = vgetq_lane_u64(Ako, 0);
    st[14] = vgetq_lane_u64(Aku, 0);
    st[15] = vgetq_lane_u64(Ama, 0); st[16] = vgetq_lane_u64(Ame, 0);
    st[17] = vgetq_lane_u64(Ami, 0); st[18] = vgetq_lane_u64(Amo, 0);
    st[19] = vgetq_lane_u64(Amu, 0);
    st[20] = vgetq_lane_u64(Asa, 0); st[21] = vgetq_lane_u64(Ase, 0);
    st[22] = vgetq_lane_u64(Asi, 0); st[23] = vgetq_lane_u64(Aso, 0);
    st[24] = vgetq_lane_u64(Asu, 0);
}

static void
keccakf_12_rounds(uint64_t st[25])
{
    uint64x2_t Aba, Abe, Abi, Abo, Abu;
    uint64x2_t Aga, Age, Agi, Ago, Agu;
    uint64x2_t Aka, Ake, Aki, Ako, Aku;
    uint64x2_t Ama, Ame, Ami, Amo, Amu;
    uint64x2_t Asa, Ase, Asi, Aso, Asu;
    uint64x2_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64x2_t Ega, Ege, Egi, Ego, Egu;
    uint64x2_t Eka, Eke, Eki, Eko, Eku;
    uint64x2_t Ema, Eme, Emi, Emo, Emu;
    uint64x2_t Esa, Ese, Esi, Eso, Esu;
    uint64x2_t Ca, Ce, Ci, Co, Cu;
    uint64x2_t Da, De, Di, Do, Du;
    uint64x2_t Ba, Be, Bi, Bo, Bu;

    Aba = vdupq_n_u64(st[0]);  Abe = vdupq_n_u64(st[1]);
    Abi = vdupq_n_u64(st[2]);  Abo = vdupq_n_u64(st[3]);
    Abu = vdupq_n_u64(st[4]);
    Aga = vdupq_n_u64(st[5]);  Age = vdupq_n_u64(st[6]);
    Agi = vdupq_n_u64(st[7]);  Ago = vdupq_n_u64(st[8]);
    Agu = vdupq_n_u64(st[9]);
    Aka = vdupq_n_u64(st[10]); Ake = vdupq_n_u64(st[11]);
    Aki = vdupq_n_u64(st[12]); Ako = vdupq_n_u64(st[13]);
    Aku = vdupq_n_u64(st[14]);
    Ama = vdupq_n_u64(st[15]); Ame = vdupq_n_u64(st[16]);
    Ami = vdupq_n_u64(st[17]); Amo = vdupq_n_u64(st[18]);
    Amu = vdupq_n_u64(st[19]);
    Asa = vdupq_n_u64(st[20]); Ase = vdupq_n_u64(st[21]);
    Asi = vdupq_n_u64(st[22]); Aso = vdupq_n_u64(st[23]);
    Asu = vdupq_n_u64(st[24]);

    ROUND(A, E, 12); ROUND(E, A, 13);
    ROUND(A, E, 14); ROUND(E, A, 15);
    ROUND(A, E, 16); ROUND(E, A, 17);
    ROUND(A, E, 18); ROUND(E, A, 19);
    ROUND(A, E, 20); ROUND(E, A, 21);
    ROUND(A, E, 22); ROUND(E, A, 23);

    st[0]  = vgetq_lane_u64(Aba, 0); st[1]  = vgetq_lane_u64(Abe, 0);
    st[2]  = vgetq_lane_u64(Abi, 0); st[3]  = vgetq_lane_u64(Abo, 0);
    st[4]  = vgetq_lane_u64(Abu, 0);
    st[5]  = vgetq_lane_u64(Aga, 0); st[6]  = vgetq_lane_u64(Age, 0);
    st[7]  = vgetq_lane_u64(Agi, 0); st[8]  = vgetq_lane_u64(Ago, 0);
    st[9]  = vgetq_lane_u64(Agu, 0);
    st[10] = vgetq_lane_u64(Aka, 0); st[11] = vgetq_lane_u64(Ake, 0);
    st[12] = vgetq_lane_u64(Aki, 0); st[13] = vgetq_lane_u64(Ako, 0);
    st[14] = vgetq_lane_u64(Aku, 0);
    st[15] = vgetq_lane_u64(Ama, 0); st[16] = vgetq_lane_u64(Ame, 0);
    st[17] = vgetq_lane_u64(Ami, 0); st[18] = vgetq_lane_u64(Amo, 0);
    st[19] = vgetq_lane_u64(Amu, 0);
    st[20] = vgetq_lane_u64(Asa, 0); st[21] = vgetq_lane_u64(Ase, 0);
    st[22] = vgetq_lane_u64(Asi, 0); st[23] = vgetq_lane_u64(Aso, 0);
    st[24] = vgetq_lane_u64(Asu, 0);
}

void
keccak1600_armsha3_permute_24(void *state)
{
    uint64_t     st[25];
    unsigned int i;

    for (i = 0U; i < 25U; i++) {
        st[i] = LOAD64_LE((const unsigned char *) state + i * 8U);
    }

    keccakf_24_rounds(st);

    for (i = 0U; i < 25U; i++) {
        STORE64_LE((unsigned char *) state + i * 8U, st[i]);
    }
}

void
keccak1600_armsha3_permute_12(void *state)
{
    uint64_t     st[25];
    unsigned int i;

    for (i = 0U; i < 25U; i++) {
        st[i] = LOAD64_LE((const unsigned char *) state + i * 8U);
    }

    keccakf_12_rounds(st);

    for (i = 0U; i < 25U; i++) {
        STORE64_LE((unsigned char *) state + i * 8U, st[i]);
    }
}

void
keccak1600_armsha3_init(void *state)
{
    memset(state, 0, KECCAK1600_STATEBYTES);
}

void
keccak1600_armsha3_xor_bytes(void *state, const unsigned char *data, size_t offset, size_t length)
{
    unsigned char *st = (unsigned char *) state;
    size_t         i;

    for (i = 0U; i < length; i++) {
        st[offset + i] ^= data[i];
    }
}

void
keccak1600_armsha3_extract_bytes(const void *state, unsigned char *data, size_t offset, size_t length)
{
    const unsigned char *st = (const unsigned char *) state;

    memcpy(data, st + offset, length);
}

#ifdef __clang__
# pragma clang attribute pop
#endif

#endif /* __ARM_FEATURE_SHA3 */
