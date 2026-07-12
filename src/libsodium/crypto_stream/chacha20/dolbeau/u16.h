
#define VEC16_LINE1(A, B, C, D)             \
    x_##A = _mm512_add_epi32(x_##A, x_##B); \
    x_##D = _mm512_rol_epi32(_mm512_xor_si512(x_##D, x_##A), 16)
#define VEC16_LINE2(A, B, C, D)             \
    x_##C = _mm512_add_epi32(x_##C, x_##D); \
    x_##B = _mm512_rol_epi32(_mm512_xor_si512(x_##B, x_##C), 12)
#define VEC16_LINE3(A, B, C, D)             \
    x_##A = _mm512_add_epi32(x_##A, x_##B); \
    x_##D = _mm512_rol_epi32(_mm512_xor_si512(x_##D, x_##A), 8)
#define VEC16_LINE4(A, B, C, D)             \
    x_##C = _mm512_add_epi32(x_##C, x_##D); \
    x_##B = _mm512_rol_epi32(_mm512_xor_si512(x_##B, x_##C), 7)

#define VEC16_ROUND(A1, B1, C1, D1, A2, B2, C2, D2, A3, B3, C3, D3, A4, B4, C4, \
                    D4)                                                         \
    VEC16_LINE1(A1, B1, C1, D1);                                               \
    VEC16_LINE1(A2, B2, C2, D2);                                               \
    VEC16_LINE1(A3, B3, C3, D3);                                               \
    VEC16_LINE1(A4, B4, C4, D4);                                               \
    VEC16_LINE2(A1, B1, C1, D1);                                               \
    VEC16_LINE2(A2, B2, C2, D2);                                               \
    VEC16_LINE2(A3, B3, C3, D3);                                               \
    VEC16_LINE2(A4, B4, C4, D4);                                               \
    VEC16_LINE3(A1, B1, C1, D1);                                               \
    VEC16_LINE3(A2, B2, C2, D2);                                               \
    VEC16_LINE3(A3, B3, C3, D3);                                               \
    VEC16_LINE3(A4, B4, C4, D4);                                               \
    VEC16_LINE4(A1, B1, C1, D1);                                               \
    VEC16_LINE4(A2, B2, C2, D2);                                               \
    VEC16_LINE4(A3, B3, C3, D3);                                               \
    VEC16_LINE4(A4, B4, C4, D4)

/* store 4 blocks whose 16 words are spread one-per-lane across the 4 source
 * registers; a 4x4 transpose of 128-bit lanes finishes the 16x16 transpose that
 * the two unpack stages below start. */
#define ONEQUAD_TRANSPOSE_STORE(A, B, C, D)                                    \
    {                                                                          \
        __m512i u0 = _mm512_shuffle_i32x4(x_##A, x_##B, 0x88);                 \
        __m512i u1 = _mm512_shuffle_i32x4(x_##A, x_##B, 0xdd);                 \
        __m512i u2 = _mm512_shuffle_i32x4(x_##C, x_##D, 0x88);                 \
        __m512i u3 = _mm512_shuffle_i32x4(x_##C, x_##D, 0xdd);                 \
        __m512i b0 = _mm512_shuffle_i32x4(u0, u2, 0x88);                       \
        __m512i b1 = _mm512_shuffle_i32x4(u1, u3, 0x88);                       \
        __m512i b2 = _mm512_shuffle_i32x4(u0, u2, 0xdd);                       \
        __m512i b3 = _mm512_shuffle_i32x4(u1, u3, 0xdd);                       \
        b0 = _mm512_xor_si512(                                                 \
            b0, _mm512_loadu_si512((const void *) (m + 64 * (A))));            \
        b1 = _mm512_xor_si512(                                                 \
            b1, _mm512_loadu_si512((const void *) (m + 64 * (B))));            \
        b2 = _mm512_xor_si512(                                                 \
            b2, _mm512_loadu_si512((const void *) (m + 64 * (C))));            \
        b3 = _mm512_xor_si512(                                                 \
            b3, _mm512_loadu_si512((const void *) (m + 64 * (D))));            \
        _mm512_storeu_si512((void *) (c + 64 * (A)), b0);                      \
        _mm512_storeu_si512((void *) (c + 64 * (B)), b1);                      \
        _mm512_storeu_si512((void *) (c + 64 * (C)), b2);                      \
        _mm512_storeu_si512((void *) (c + 64 * (D)), b3);                      \
    }

if (bytes >= 1024) {
    const __m512i addv    = _mm512_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                              11, 12, 13, 14, 15);
    uint32_t      in12, in13;

    __m512i x_0  = _mm512_set1_epi32(x[0]);
    __m512i x_1  = _mm512_set1_epi32(x[1]);
    __m512i x_2  = _mm512_set1_epi32(x[2]);
    __m512i x_3  = _mm512_set1_epi32(x[3]);
    __m512i x_4  = _mm512_set1_epi32(x[4]);
    __m512i x_5  = _mm512_set1_epi32(x[5]);
    __m512i x_6  = _mm512_set1_epi32(x[6]);
    __m512i x_7  = _mm512_set1_epi32(x[7]);
    __m512i x_8  = _mm512_set1_epi32(x[8]);
    __m512i x_9  = _mm512_set1_epi32(x[9]);
    __m512i x_10 = _mm512_set1_epi32(x[10]);
    __m512i x_11 = _mm512_set1_epi32(x[11]);
    __m512i x_12;
    __m512i x_13;
    __m512i x_14 = _mm512_set1_epi32(x[14]);
    __m512i x_15 = _mm512_set1_epi32(x[15]);

    __m512i orig0  = x_0;
    __m512i orig1  = x_1;
    __m512i orig2  = x_2;
    __m512i orig3  = x_3;
    __m512i orig4  = x_4;
    __m512i orig5  = x_5;
    __m512i orig6  = x_6;
    __m512i orig7  = x_7;
    __m512i orig8  = x_8;
    __m512i orig9  = x_9;
    __m512i orig10 = x_10;
    __m512i orig11 = x_11;
    __m512i orig12;
    __m512i orig13;
    __m512i orig14 = x_14;
    __m512i orig15 = x_15;
    __m512i t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7, t_8, t_9, t_10, t_11, t_12,
        t_13, t_14, t_15;

    while (bytes >= 1024) {
        __m512i   base12, base13;
        __mmask16 carry;
        uint64_t  in1213;
        int       i;

        x_0  = orig0;
        x_1  = orig1;
        x_2  = orig2;
        x_3  = orig3;
        x_4  = orig4;
        x_5  = orig5;
        x_6  = orig6;
        x_7  = orig7;
        x_8  = orig8;
        x_9  = orig9;
        x_10 = orig10;
        x_11 = orig11;
        x_14 = orig14;
        x_15 = orig15;

        in12 = x[12];
        in13 = x[13];

        /* per-block 64-bit counter: lane j holds (in12|in13<<32) + j, split back
         * into low/high words. carry propagates only where the low add wraps. */
        base12 = _mm512_set1_epi32(in12);
        base13 = _mm512_set1_epi32(in13);
        x_12   = _mm512_add_epi32(base12, addv);
        carry  = _mm512_cmplt_epu32_mask(x_12, base12);
        x_13   = _mm512_mask_add_epi32(base13, carry, base13,
                                       _mm512_set1_epi32(1));

        orig12 = x_12;
        orig13 = x_13;

        in1213 = ((uint64_t) in12) | (((uint64_t) in13) << 32);
        in1213 += 16;

        x[12] = in1213 & 0xFFFFFFFF;
        x[13] = (in1213 >> 32) & 0xFFFFFFFF;

        for (i = 0; i < ROUNDS; i += 2) {
            VEC16_ROUND(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
            VEC16_ROUND(0, 5, 10, 15, 1, 6, 11, 12, 2, 7, 8, 13, 3, 4, 9, 14);
        }

        x_0  = _mm512_add_epi32(x_0, orig0);
        x_1  = _mm512_add_epi32(x_1, orig1);
        x_2  = _mm512_add_epi32(x_2, orig2);
        x_3  = _mm512_add_epi32(x_3, orig3);
        x_4  = _mm512_add_epi32(x_4, orig4);
        x_5  = _mm512_add_epi32(x_5, orig5);
        x_6  = _mm512_add_epi32(x_6, orig6);
        x_7  = _mm512_add_epi32(x_7, orig7);
        x_8  = _mm512_add_epi32(x_8, orig8);
        x_9  = _mm512_add_epi32(x_9, orig9);
        x_10 = _mm512_add_epi32(x_10, orig10);
        x_11 = _mm512_add_epi32(x_11, orig11);
        x_12 = _mm512_add_epi32(x_12, orig12);
        x_13 = _mm512_add_epi32(x_13, orig13);
        x_14 = _mm512_add_epi32(x_14, orig14);
        x_15 = _mm512_add_epi32(x_15, orig15);

        /* 16x16 transpose, stage 1: interleave 32-bit words of adjacent rows */
        t_0  = _mm512_unpacklo_epi32(x_0, x_1);
        t_1  = _mm512_unpackhi_epi32(x_0, x_1);
        t_2  = _mm512_unpacklo_epi32(x_2, x_3);
        t_3  = _mm512_unpackhi_epi32(x_2, x_3);
        t_4  = _mm512_unpacklo_epi32(x_4, x_5);
        t_5  = _mm512_unpackhi_epi32(x_4, x_5);
        t_6  = _mm512_unpacklo_epi32(x_6, x_7);
        t_7  = _mm512_unpackhi_epi32(x_6, x_7);
        t_8  = _mm512_unpacklo_epi32(x_8, x_9);
        t_9  = _mm512_unpackhi_epi32(x_8, x_9);
        t_10 = _mm512_unpacklo_epi32(x_10, x_11);
        t_11 = _mm512_unpackhi_epi32(x_10, x_11);
        t_12 = _mm512_unpacklo_epi32(x_12, x_13);
        t_13 = _mm512_unpackhi_epi32(x_12, x_13);
        t_14 = _mm512_unpacklo_epi32(x_14, x_15);
        t_15 = _mm512_unpackhi_epi32(x_14, x_15);

        /* stage 2: interleave 64-bit words */
        x_0  = _mm512_unpacklo_epi64(t_0, t_2);
        x_1  = _mm512_unpackhi_epi64(t_0, t_2);
        x_2  = _mm512_unpacklo_epi64(t_1, t_3);
        x_3  = _mm512_unpackhi_epi64(t_1, t_3);
        x_4  = _mm512_unpacklo_epi64(t_4, t_6);
        x_5  = _mm512_unpackhi_epi64(t_4, t_6);
        x_6  = _mm512_unpacklo_epi64(t_5, t_7);
        x_7  = _mm512_unpackhi_epi64(t_5, t_7);
        x_8  = _mm512_unpacklo_epi64(t_8, t_10);
        x_9  = _mm512_unpackhi_epi64(t_8, t_10);
        x_10 = _mm512_unpacklo_epi64(t_9, t_11);
        x_11 = _mm512_unpackhi_epi64(t_9, t_11);
        x_12 = _mm512_unpacklo_epi64(t_12, t_14);
        x_13 = _mm512_unpackhi_epi64(t_12, t_14);
        x_14 = _mm512_unpacklo_epi64(t_13, t_15);
        x_15 = _mm512_unpackhi_epi64(t_13, t_15);

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

#undef VEC16_LINE1
#undef VEC16_LINE2
#undef VEC16_LINE3
#undef VEC16_LINE4
#undef VEC16_ROUND
#undef ONEQUAD_TRANSPOSE_STORE
