#define ONEQUAD_TRANSPOSE_STORE(A, B, C, D)                                   \
    {                                                                         \
        __m512i u0 = _mm512_shuffle_i32x4(z##A, z##B, 0x88);                  \
        __m512i u1 = _mm512_shuffle_i32x4(z##A, z##B, 0xdd);                  \
        __m512i u2 = _mm512_shuffle_i32x4(z##C, z##D, 0x88);                  \
        __m512i u3 = _mm512_shuffle_i32x4(z##C, z##D, 0xdd);                  \
        __m512i b0 = _mm512_shuffle_i32x4(u0, u2, 0x88);                      \
        __m512i b1 = _mm512_shuffle_i32x4(u1, u3, 0x88);                      \
        __m512i b2 = _mm512_shuffle_i32x4(u0, u2, 0xdd);                      \
        __m512i b3 = _mm512_shuffle_i32x4(u1, u3, 0xdd);                      \
        b0 = _mm512_xor_si512(                                                \
            b0, _mm512_loadu_si512((const void *) (m + 64 * (A))));           \
        b1 = _mm512_xor_si512(                                                \
            b1, _mm512_loadu_si512((const void *) (m + 64 * (B))));           \
        b2 = _mm512_xor_si512(                                                \
            b2, _mm512_loadu_si512((const void *) (m + 64 * (C))));           \
        b3 = _mm512_xor_si512(                                                \
            b3, _mm512_loadu_si512((const void *) (m + 64 * (D))));           \
        _mm512_storeu_si512((void *) (c + 64 * (A)), b0);                     \
        _mm512_storeu_si512((void *) (c + 64 * (B)), b1);                     \
        _mm512_storeu_si512((void *) (c + 64 * (C)), b2);                     \
        _mm512_storeu_si512((void *) (c + 64 * (D)), b3);                     \
    }

if (bytes >= 1024) {
    const __m512i addv = _mm512_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                           12, 13, 14, 15);
    __m512i y0, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13, y14, y15;

    __m512i z0  = _mm512_set1_epi32(x[0]);
    __m512i z5  = _mm512_set1_epi32(x[1]);
    __m512i z10 = _mm512_set1_epi32(x[2]);
    __m512i z15 = _mm512_set1_epi32(x[3]);
    __m512i z12 = _mm512_set1_epi32(x[4]);
    __m512i z1  = _mm512_set1_epi32(x[5]);
    __m512i z6  = _mm512_set1_epi32(x[6]);
    __m512i z11 = _mm512_set1_epi32(x[7]);
    __m512i z8;
    __m512i z13 = _mm512_set1_epi32(x[9]);
    __m512i z2  = _mm512_set1_epi32(x[10]);
    __m512i z7  = _mm512_set1_epi32(x[11]);
    __m512i z4  = _mm512_set1_epi32(x[12]);
    __m512i z9;
    __m512i z14 = _mm512_set1_epi32(x[14]);
    __m512i z3  = _mm512_set1_epi32(x[15]);

    __m512i orig0 = z0;
    __m512i orig1 = z1;
    __m512i orig2 = z2;
    __m512i orig3 = z3;
    __m512i orig4 = z4;
    __m512i orig5 = z5;
    __m512i orig6 = z6;
    __m512i orig7 = z7;
    __m512i orig8;
    __m512i orig9;
    __m512i orig10 = z10;
    __m512i orig11 = z11;
    __m512i orig12 = z12;
    __m512i orig13 = z13;
    __m512i orig14 = z14;
    __m512i orig15 = z15;

    uint32_t in8;
    uint32_t in9;
    int      i;

    while (bytes >= 1024) {
        __m512i   base8, base9;
        __mmask16 carry;
        uint64_t  in89;

        /* per-block 64-bit counter (matrix words 8 and 9, stored at x[8]/x[13]):
         * lane j holds base + j split into low/high; the high word takes a carry
         * only where the low add wraps. */
        in8   = x[8];
        in9   = x[13];
        base8 = _mm512_set1_epi32(in8);
        base9 = _mm512_set1_epi32(in9);
        z8    = _mm512_add_epi32(base8, addv);
        carry = _mm512_cmplt_epu32_mask(z8, base8);
        z9    = _mm512_mask_add_epi32(base9, carry, base9,
                                      _mm512_set1_epi32(1));

        orig8 = z8;
        orig9 = z9;

        in89  = ((uint64_t) in8) | (((uint64_t) in9) << 32);
        in89 += 16;

        x[8]  = in89 & 0xFFFFFFFF;
        x[13] = (in89 >> 32) & 0xFFFFFFFF;

        z0  = orig0;
        z1  = orig1;
        z2  = orig2;
        z3  = orig3;
        z4  = orig4;
        z5  = orig5;
        z6  = orig6;
        z7  = orig7;
        z10 = orig10;
        z11 = orig11;
        z12 = orig12;
        z13 = orig13;
        z14 = orig14;
        z15 = orig15;

        for (i = 0; i < ROUNDS; i += 2) {
            z4 = _mm512_xor_si512(
                z4, _mm512_rol_epi32(_mm512_add_epi32(z12, z0), 7));
            z9 = _mm512_xor_si512(
                z9, _mm512_rol_epi32(_mm512_add_epi32(z1, z5), 7));
            z8 = _mm512_xor_si512(
                z8, _mm512_rol_epi32(_mm512_add_epi32(z0, z4), 9));
            z13 = _mm512_xor_si512(
                z13, _mm512_rol_epi32(_mm512_add_epi32(z5, z9), 9));
            z12 = _mm512_xor_si512(
                z12, _mm512_rol_epi32(_mm512_add_epi32(z4, z8), 13));
            z1 = _mm512_xor_si512(
                z1, _mm512_rol_epi32(_mm512_add_epi32(z9, z13), 13));
            z0 = _mm512_xor_si512(
                z0, _mm512_rol_epi32(_mm512_add_epi32(z8, z12), 18));
            z5 = _mm512_xor_si512(
                z5, _mm512_rol_epi32(_mm512_add_epi32(z13, z1), 18));
            z14 = _mm512_xor_si512(
                z14, _mm512_rol_epi32(_mm512_add_epi32(z6, z10), 7));
            z3 = _mm512_xor_si512(
                z3, _mm512_rol_epi32(_mm512_add_epi32(z11, z15), 7));
            z2 = _mm512_xor_si512(
                z2, _mm512_rol_epi32(_mm512_add_epi32(z10, z14), 9));
            z7 = _mm512_xor_si512(
                z7, _mm512_rol_epi32(_mm512_add_epi32(z15, z3), 9));
            z6 = _mm512_xor_si512(
                z6, _mm512_rol_epi32(_mm512_add_epi32(z14, z2), 13));
            z11 = _mm512_xor_si512(
                z11, _mm512_rol_epi32(_mm512_add_epi32(z3, z7), 13));
            z10 = _mm512_xor_si512(
                z10, _mm512_rol_epi32(_mm512_add_epi32(z2, z6), 18));
            z1 = _mm512_xor_si512(
                z1, _mm512_rol_epi32(_mm512_add_epi32(z3, z0), 7));
            z15 = _mm512_xor_si512(
                z15, _mm512_rol_epi32(_mm512_add_epi32(z7, z11), 18));
            z6 = _mm512_xor_si512(
                z6, _mm512_rol_epi32(_mm512_add_epi32(z4, z5), 7));
            z2 = _mm512_xor_si512(
                z2, _mm512_rol_epi32(_mm512_add_epi32(z0, z1), 9));
            z7 = _mm512_xor_si512(
                z7, _mm512_rol_epi32(_mm512_add_epi32(z5, z6), 9));
            z3 = _mm512_xor_si512(
                z3, _mm512_rol_epi32(_mm512_add_epi32(z1, z2), 13));
            z4 = _mm512_xor_si512(
                z4, _mm512_rol_epi32(_mm512_add_epi32(z6, z7), 13));
            z0 = _mm512_xor_si512(
                z0, _mm512_rol_epi32(_mm512_add_epi32(z2, z3), 18));
            z5 = _mm512_xor_si512(
                z5, _mm512_rol_epi32(_mm512_add_epi32(z7, z4), 18));
            z11 = _mm512_xor_si512(
                z11, _mm512_rol_epi32(_mm512_add_epi32(z9, z10), 7));
            z12 = _mm512_xor_si512(
                z12, _mm512_rol_epi32(_mm512_add_epi32(z14, z15), 7));
            z8 = _mm512_xor_si512(
                z8, _mm512_rol_epi32(_mm512_add_epi32(z10, z11), 9));
            z13 = _mm512_xor_si512(
                z13, _mm512_rol_epi32(_mm512_add_epi32(z15, z12), 9));
            z9 = _mm512_xor_si512(
                z9, _mm512_rol_epi32(_mm512_add_epi32(z11, z8), 13));
            z14 = _mm512_xor_si512(
                z14, _mm512_rol_epi32(_mm512_add_epi32(z12, z13), 13));
            z10 = _mm512_xor_si512(
                z10, _mm512_rol_epi32(_mm512_add_epi32(z8, z9), 18));
            z15 = _mm512_xor_si512(
                z15, _mm512_rol_epi32(_mm512_add_epi32(z13, z14), 18));
        }

        z0  = _mm512_add_epi32(z0, orig0);
        z1  = _mm512_add_epi32(z1, orig1);
        z2  = _mm512_add_epi32(z2, orig2);
        z3  = _mm512_add_epi32(z3, orig3);
        z4  = _mm512_add_epi32(z4, orig4);
        z5  = _mm512_add_epi32(z5, orig5);
        z6  = _mm512_add_epi32(z6, orig6);
        z7  = _mm512_add_epi32(z7, orig7);
        z8  = _mm512_add_epi32(z8, orig8);
        z9  = _mm512_add_epi32(z9, orig9);
        z10 = _mm512_add_epi32(z10, orig10);
        z11 = _mm512_add_epi32(z11, orig11);
        z12 = _mm512_add_epi32(z12, orig12);
        z13 = _mm512_add_epi32(z13, orig13);
        z14 = _mm512_add_epi32(z14, orig14);
        z15 = _mm512_add_epi32(z15, orig15);

        /* 16x16 transpose, stage 1: interleave 32-bit words of adjacent rows */
        y0  = _mm512_unpacklo_epi32(z0, z1);
        y1  = _mm512_unpackhi_epi32(z0, z1);
        y2  = _mm512_unpacklo_epi32(z2, z3);
        y3  = _mm512_unpackhi_epi32(z2, z3);
        y4  = _mm512_unpacklo_epi32(z4, z5);
        y5  = _mm512_unpackhi_epi32(z4, z5);
        y6  = _mm512_unpacklo_epi32(z6, z7);
        y7  = _mm512_unpackhi_epi32(z6, z7);
        y8  = _mm512_unpacklo_epi32(z8, z9);
        y9  = _mm512_unpackhi_epi32(z8, z9);
        y10 = _mm512_unpacklo_epi32(z10, z11);
        y11 = _mm512_unpackhi_epi32(z10, z11);
        y12 = _mm512_unpacklo_epi32(z12, z13);
        y13 = _mm512_unpackhi_epi32(z12, z13);
        y14 = _mm512_unpacklo_epi32(z14, z15);
        y15 = _mm512_unpackhi_epi32(z14, z15);

        /* stage 2: interleave 64-bit words */
        z0  = _mm512_unpacklo_epi64(y0, y2);
        z1  = _mm512_unpackhi_epi64(y0, y2);
        z2  = _mm512_unpacklo_epi64(y1, y3);
        z3  = _mm512_unpackhi_epi64(y1, y3);
        z4  = _mm512_unpacklo_epi64(y4, y6);
        z5  = _mm512_unpackhi_epi64(y4, y6);
        z6  = _mm512_unpacklo_epi64(y5, y7);
        z7  = _mm512_unpackhi_epi64(y5, y7);
        z8  = _mm512_unpacklo_epi64(y8, y10);
        z9  = _mm512_unpackhi_epi64(y8, y10);
        z10 = _mm512_unpacklo_epi64(y9, y11);
        z11 = _mm512_unpackhi_epi64(y9, y11);
        z12 = _mm512_unpacklo_epi64(y12, y14);
        z13 = _mm512_unpackhi_epi64(y12, y14);
        z14 = _mm512_unpacklo_epi64(y13, y15);
        z15 = _mm512_unpackhi_epi64(y13, y15);

        /* stage 3/4: shuffle 128-bit lanes, xor with the message, store */
        ONEQUAD_TRANSPOSE_STORE(0, 4, 8, 12);
        ONEQUAD_TRANSPOSE_STORE(1, 5, 9, 13);
        ONEQUAD_TRANSPOSE_STORE(2, 6, 10, 14);
        ONEQUAD_TRANSPOSE_STORE(3, 7, 11, 15);

        bytes -= 1024;
        c += 1024;
        m += 1024;
    }
}

#undef ONEQUAD_TRANSPOSE_STORE
