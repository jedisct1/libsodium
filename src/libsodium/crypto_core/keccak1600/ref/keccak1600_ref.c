#include <stdint.h>
#include <string.h>

#include "keccak1600_ref.h"
#include "private/common.h"

#define KECCAK1600_STATEBYTES 200

static const uint64_t keccak_round_constants[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
    0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define KECCAK_DECLARE_STATE          \
    uint64_t Aba, Abe, Abi, Abo, Abu; \
    uint64_t Aga, Age, Agi, Ago, Agu; \
    uint64_t Aka, Ake, Aki, Ako, Aku; \
    uint64_t Ama, Ame, Ami, Amo, Amu; \
    uint64_t Asa, Ase, Asi, Aso, Asu; \
    uint64_t Bba, Bbe, Bbi, Bbo, Bbu; \
    uint64_t Bga, Bge, Bgi, Bgo, Bgu; \
    uint64_t Bka, Bke, Bki, Bko, Bku; \
    uint64_t Bma, Bme, Bmi, Bmo, Bmu; \
    uint64_t Bsa, Bse, Bsi, Bso, Bsu; \
    uint64_t Ca, Ce, Ci, Co, Cu;      \
    uint64_t Da, De, Di, Do, Du;      \
    uint64_t Eba, Ebe, Ebi, Ebo, Ebu; \
    uint64_t Ega, Ege, Egi, Ego, Egu; \
    uint64_t Eka, Eke, Eki, Eko, Eku; \
    uint64_t Ema, Eme, Emi, Emo, Emu; \
    uint64_t Esa, Ese, Esi, Eso, Esu

#define KECCAK_PREPARE_THETA          \
    Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa; \
    Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase; \
    Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi; \
    Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso; \
    Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu

#define KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(round_idx, A, E) \
    Da = Cu ^ ROTL64(Ce, 1);                              \
    De = Ca ^ ROTL64(Ci, 1);                              \
    Di = Ce ^ ROTL64(Co, 1);                              \
    Do = Ci ^ ROTL64(Cu, 1);                              \
    Du = Co ^ ROTL64(Ca, 1);                              \
                                                          \
    A##ba ^= Da;                                          \
    Bba = A##ba;                                          \
    A##ge ^= De;                                          \
    Bbe = ROTL64(A##ge, 44);                              \
    A##ki ^= Di;                                          \
    Bbi = ROTL64(A##ki, 43);                              \
    A##mo ^= Do;                                          \
    Bbo = ROTL64(A##mo, 21);                              \
    A##su ^= Du;                                          \
    Bbu   = ROTL64(A##su, 14);                            \
    E##ba = Bba ^ ((~Bbe) & Bbi);                         \
    E##ba ^= keccak_round_constants[round_idx];           \
    Ca    = E##ba;                                        \
    E##be = Bbe ^ ((~Bbi) & Bbo);                         \
    Ce    = E##be;                                        \
    E##bi = Bbi ^ ((~Bbo) & Bbu);                         \
    Ci    = E##bi;                                        \
    E##bo = Bbo ^ ((~Bbu) & Bba);                         \
    Co    = E##bo;                                        \
    E##bu = Bbu ^ ((~Bba) & Bbe);                         \
    Cu    = E##bu;                                        \
                                                          \
    A##bo ^= Do;                                          \
    Bga = ROTL64(A##bo, 28);                              \
    A##gu ^= Du;                                          \
    Bge = ROTL64(A##gu, 20);                              \
    A##ka ^= Da;                                          \
    Bgi = ROTL64(A##ka, 3);                               \
    A##me ^= De;                                          \
    Bgo = ROTL64(A##me, 45);                              \
    A##si ^= Di;                                          \
    Bgu   = ROTL64(A##si, 61);                            \
    E##ga = Bga ^ ((~Bge) & Bgi);                         \
    Ca ^= E##ga;                                          \
    E##ge = Bge ^ ((~Bgi) & Bgo);                         \
    Ce ^= E##ge;                                          \
    E##gi = Bgi ^ ((~Bgo) & Bgu);                         \
    Ci ^= E##gi;                                          \
    E##go = Bgo ^ ((~Bgu) & Bga);                         \
    Co ^= E##go;                                          \
    E##gu = Bgu ^ ((~Bga) & Bge);                         \
    Cu ^= E##gu;                                          \
                                                          \
    A##be ^= De;                                          \
    Bka = ROTL64(A##be, 1);                               \
    A##gi ^= Di;                                          \
    Bke = ROTL64(A##gi, 6);                               \
    A##ko ^= Do;                                          \
    Bki = ROTL64(A##ko, 25);                              \
    A##mu ^= Du;                                          \
    Bko = ROTL64(A##mu, 8);                               \
    A##sa ^= Da;                                          \
    Bku   = ROTL64(A##sa, 18);                            \
    E##ka = Bka ^ ((~Bke) & Bki);                         \
    Ca ^= E##ka;                                          \
    E##ke = Bke ^ ((~Bki) & Bko);                         \
    Ce ^= E##ke;                                          \
    E##ki = Bki ^ ((~Bko) & Bku);                         \
    Ci ^= E##ki;                                          \
    E##ko = Bko ^ ((~Bku) & Bka);                         \
    Co ^= E##ko;                                          \
    E##ku = Bku ^ ((~Bka) & Bke);                         \
    Cu ^= E##ku;                                          \
                                                          \
    A##bu ^= Du;                                          \
    Bma = ROTL64(A##bu, 27);                              \
    A##ga ^= Da;                                          \
    Bme = ROTL64(A##ga, 36);                              \
    A##ke ^= De;                                          \
    Bmi = ROTL64(A##ke, 10);                              \
    A##mi ^= Di;                                          \
    Bmo = ROTL64(A##mi, 15);                              \
    A##so ^= Do;                                          \
    Bmu   = ROTL64(A##so, 56);                            \
    E##ma = Bma ^ ((~Bme) & Bmi);                         \
    Ca ^= E##ma;                                          \
    E##me = Bme ^ ((~Bmi) & Bmo);                         \
    Ce ^= E##me;                                          \
    E##mi = Bmi ^ ((~Bmo) & Bmu);                         \
    Ci ^= E##mi;                                          \
    E##mo = Bmo ^ ((~Bmu) & Bma);                         \
    Co ^= E##mo;                                          \
    E##mu = Bmu ^ ((~Bma) & Bme);                         \
    Cu ^= E##mu;                                          \
                                                          \
    A##bi ^= Di;                                          \
    Bsa = ROTL64(A##bi, 62);                              \
    A##go ^= Do;                                          \
    Bse = ROTL64(A##go, 55);                              \
    A##ku ^= Du;                                          \
    Bsi = ROTL64(A##ku, 39);                              \
    A##ma ^= Da;                                          \
    Bso = ROTL64(A##ma, 41);                              \
    A##se ^= De;                                          \
    Bsu   = ROTL64(A##se, 2);                             \
    E##sa = Bsa ^ ((~Bse) & Bsi);                         \
    Ca ^= E##sa;                                          \
    E##se = Bse ^ ((~Bsi) & Bso);                         \
    Ce ^= E##se;                                          \
    E##si = Bsi ^ ((~Bso) & Bsu);                         \
    Ci ^= E##si;                                          \
    E##so = Bso ^ ((~Bsu) & Bsa);                         \
    Co ^= E##so;                                          \
    E##su = Bsu ^ ((~Bsa) & Bse);                         \
    Cu ^= E##su

#define KECCAK_THETA_RHO_PI_CHI_IOTA(round_idx, A, E) \
    Da = Cu ^ ROTL64(Ce, 1);                          \
    De = Ca ^ ROTL64(Ci, 1);                          \
    Di = Ce ^ ROTL64(Co, 1);                          \
    Do = Ci ^ ROTL64(Cu, 1);                          \
    Du = Co ^ ROTL64(Ca, 1);                          \
                                                      \
    A##ba ^= Da;                                      \
    Bba = A##ba;                                      \
    A##ge ^= De;                                      \
    Bbe = ROTL64(A##ge, 44);                          \
    A##ki ^= Di;                                      \
    Bbi = ROTL64(A##ki, 43);                          \
    A##mo ^= Do;                                      \
    Bbo = ROTL64(A##mo, 21);                          \
    A##su ^= Du;                                      \
    Bbu   = ROTL64(A##su, 14);                        \
    E##ba = Bba ^ ((~Bbe) & Bbi);                     \
    E##ba ^= keccak_round_constants[round_idx];       \
    E##be = Bbe ^ ((~Bbi) & Bbo);                     \
    E##bi = Bbi ^ ((~Bbo) & Bbu);                     \
    E##bo = Bbo ^ ((~Bbu) & Bba);                     \
    E##bu = Bbu ^ ((~Bba) & Bbe);                     \
                                                      \
    A##bo ^= Do;                                      \
    Bga = ROTL64(A##bo, 28);                          \
    A##gu ^= Du;                                      \
    Bge = ROTL64(A##gu, 20);                          \
    A##ka ^= Da;                                      \
    Bgi = ROTL64(A##ka, 3);                           \
    A##me ^= De;                                      \
    Bgo = ROTL64(A##me, 45);                          \
    A##si ^= Di;                                      \
    Bgu   = ROTL64(A##si, 61);                        \
    E##ga = Bga ^ ((~Bge) & Bgi);                     \
    E##ge = Bge ^ ((~Bgi) & Bgo);                     \
    E##gi = Bgi ^ ((~Bgo) & Bgu);                     \
    E##go = Bgo ^ ((~Bgu) & Bga);                     \
    E##gu = Bgu ^ ((~Bga) & Bge);                     \
                                                      \
    A##be ^= De;                                      \
    Bka = ROTL64(A##be, 1);                           \
    A##gi ^= Di;                                      \
    Bke = ROTL64(A##gi, 6);                           \
    A##ko ^= Do;                                      \
    Bki = ROTL64(A##ko, 25);                          \
    A##mu ^= Du;                                      \
    Bko = ROTL64(A##mu, 8);                           \
    A##sa ^= Da;                                      \
    Bku   = ROTL64(A##sa, 18);                        \
    E##ka = Bka ^ ((~Bke) & Bki);                     \
    E##ke = Bke ^ ((~Bki) & Bko);                     \
    E##ki = Bki ^ ((~Bko) & Bku);                     \
    E##ko = Bko ^ ((~Bku) & Bka);                     \
    E##ku = Bku ^ ((~Bka) & Bke);                     \
                                                      \
    A##bu ^= Du;                                      \
    Bma = ROTL64(A##bu, 27);                          \
    A##ga ^= Da;                                      \
    Bme = ROTL64(A##ga, 36);                          \
    A##ke ^= De;                                      \
    Bmi = ROTL64(A##ke, 10);                          \
    A##mi ^= Di;                                      \
    Bmo = ROTL64(A##mi, 15);                          \
    A##so ^= Do;                                      \
    Bmu   = ROTL64(A##so, 56);                        \
    E##ma = Bma ^ ((~Bme) & Bmi);                     \
    E##me = Bme ^ ((~Bmi) & Bmo);                     \
    E##mi = Bmi ^ ((~Bmo) & Bmu);                     \
    E##mo = Bmo ^ ((~Bmu) & Bma);                     \
    E##mu = Bmu ^ ((~Bma) & Bme);                     \
                                                      \
    A##bi ^= Di;                                      \
    Bsa = ROTL64(A##bi, 62);                          \
    A##go ^= Do;                                      \
    Bse = ROTL64(A##go, 55);                          \
    A##ku ^= Du;                                      \
    Bsi = ROTL64(A##ku, 39);                          \
    A##ma ^= Da;                                      \
    Bso = ROTL64(A##ma, 41);                          \
    A##se ^= De;                                      \
    Bsu   = ROTL64(A##se, 2);                         \
    E##sa = Bsa ^ ((~Bse) & Bsi);                     \
    E##se = Bse ^ ((~Bsi) & Bso);                     \
    E##si = Bsi ^ ((~Bso) & Bsu);                     \
    E##so = Bso ^ ((~Bsu) & Bsa);                     \
    E##su = Bsu ^ ((~Bsa) & Bse)

#define KECCAK_COPY_FROM_STATE(prefix, src) \
    prefix##ba = (src)[0];                  \
    prefix##be = (src)[1];                  \
    prefix##bi = (src)[2];                  \
    prefix##bo = (src)[3];                  \
    prefix##bu = (src)[4];                  \
    prefix##ga = (src)[5];                  \
    prefix##ge = (src)[6];                  \
    prefix##gi = (src)[7];                  \
    prefix##go = (src)[8];                  \
    prefix##gu = (src)[9];                  \
    prefix##ka = (src)[10];                 \
    prefix##ke = (src)[11];                 \
    prefix##ki = (src)[12];                 \
    prefix##ko = (src)[13];                 \
    prefix##ku = (src)[14];                 \
    prefix##ma = (src)[15];                 \
    prefix##me = (src)[16];                 \
    prefix##mi = (src)[17];                 \
    prefix##mo = (src)[18];                 \
    prefix##mu = (src)[19];                 \
    prefix##sa = (src)[20];                 \
    prefix##se = (src)[21];                 \
    prefix##si = (src)[22];                 \
    prefix##so = (src)[23];                 \
    prefix##su = (src)[24]

#define KECCAK_COPY_TO_STATE(dst, prefix) \
    (dst)[0]  = prefix##ba;               \
    (dst)[1]  = prefix##be;               \
    (dst)[2]  = prefix##bi;               \
    (dst)[3]  = prefix##bo;               \
    (dst)[4]  = prefix##bu;               \
    (dst)[5]  = prefix##ga;               \
    (dst)[6]  = prefix##ge;               \
    (dst)[7]  = prefix##gi;               \
    (dst)[8]  = prefix##go;               \
    (dst)[9]  = prefix##gu;               \
    (dst)[10] = prefix##ka;               \
    (dst)[11] = prefix##ke;               \
    (dst)[12] = prefix##ki;               \
    (dst)[13] = prefix##ko;               \
    (dst)[14] = prefix##ku;               \
    (dst)[15] = prefix##ma;               \
    (dst)[16] = prefix##me;               \
    (dst)[17] = prefix##mi;               \
    (dst)[18] = prefix##mo;               \
    (dst)[19] = prefix##mu;               \
    (dst)[20] = prefix##sa;               \
    (dst)[21] = prefix##se;               \
    (dst)[22] = prefix##si;               \
    (dst)[23] = prefix##so;               \
    (dst)[24] = prefix##su

#define KECCAK_COPY_STATE(prefix_dst, prefix_src) \
    prefix_dst##ba = prefix_src##ba;              \
    prefix_dst##be = prefix_src##be;              \
    prefix_dst##bi = prefix_src##bi;              \
    prefix_dst##bo = prefix_src##bo;              \
    prefix_dst##bu = prefix_src##bu;              \
    prefix_dst##ga = prefix_src##ga;              \
    prefix_dst##ge = prefix_src##ge;              \
    prefix_dst##gi = prefix_src##gi;              \
    prefix_dst##go = prefix_src##go;              \
    prefix_dst##gu = prefix_src##gu;              \
    prefix_dst##ka = prefix_src##ka;              \
    prefix_dst##ke = prefix_src##ke;              \
    prefix_dst##ki = prefix_src##ki;              \
    prefix_dst##ko = prefix_src##ko;              \
    prefix_dst##ku = prefix_src##ku;              \
    prefix_dst##ma = prefix_src##ma;              \
    prefix_dst##me = prefix_src##me;              \
    prefix_dst##mi = prefix_src##mi;              \
    prefix_dst##mo = prefix_src##mo;              \
    prefix_dst##mu = prefix_src##mu;              \
    prefix_dst##sa = prefix_src##sa;              \
    prefix_dst##se = prefix_src##se;              \
    prefix_dst##si = prefix_src##si;              \
    prefix_dst##so = prefix_src##so;              \
    prefix_dst##su = prefix_src##su

static void
keccakf_24_rounds(uint64_t st[25])
{
    uint64_t *state = st;

    KECCAK_DECLARE_STATE;

    KECCAK_COPY_FROM_STATE(A, state);
    KECCAK_PREPARE_THETA;

    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(0, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(1, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(2, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(3, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(4, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(5, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(6, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(7, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(8, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(9, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(10, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(11, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(12, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(13, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(14, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(15, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(16, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(17, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(18, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(19, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(20, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(21, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(22, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA(23, E, A);

    KECCAK_COPY_TO_STATE(state, A);
}

static void
keccakf_12_rounds(uint64_t st[25])
{
    uint64_t *state = st;

    KECCAK_DECLARE_STATE;

    KECCAK_COPY_FROM_STATE(A, state);
    KECCAK_PREPARE_THETA;

    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(12, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(13, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(14, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(15, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(16, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(17, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(18, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(19, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(20, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(21, E, A);
    KECCAK_THETA_RHO_PI_CHI_IOTA_PRE(22, A, E);
    KECCAK_THETA_RHO_PI_CHI_IOTA(23, E, A);

    KECCAK_COPY_TO_STATE(state, A);
}

void
keccak1600_ref_permute_24(void *state)
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
keccak1600_ref_permute_12(void *state)
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
keccak1600_ref_init(void *state)
{
    memset(state, 0, KECCAK1600_STATEBYTES);
}

void
keccak1600_ref_xor_bytes(void *state, const unsigned char *data, size_t offset, size_t length)
{
    unsigned char *st = (unsigned char *) state;
    size_t         i;

    for (i = 0U; i < length; i++) {
        st[offset + i] ^= data[i];
    }
}

void
keccak1600_ref_extract_bytes(const void *state, unsigned char *data, size_t offset, size_t length)
{
    const unsigned char *st = (const unsigned char *) state;

    memcpy(data, st + offset, length);
}
