#define TEST_NAME "musig"
#include "cmptest.h"

static unsigned char m[0x1000];
static unsigned long long m_len;

static unsigned char sm[crypto_musig_BYTES + sizeof m];
static unsigned long long sm_len;
#define NSIGNERS 10

static crypto_musig_state st[NSIGNERS];
static unsigned char sk[NSIGNERS][crypto_musig_SECRETKEYBYTES];
static unsigned char pk[NSIGNERS][crypto_musig_PUBLICKEYBYTES];
static unsigned char r[NSIGNERS][crypto_musig_RBYTES];
static unsigned char s[NSIGNERS][crypto_musig_KSTBYTES];
static unsigned char t[NSIGNERS][crypto_musig_KSTBYTES];
static unsigned char k[NSIGNERS][crypto_musig_KSTBYTES];
static unsigned char mk[NSIGNERS][crypto_musig_PUBLICKEYBYTES];
static unsigned char sg[NSIGNERS][crypto_musig_BYTES];

static unsigned char test_pk[3][crypto_musig_PUBLICKEYBYTES] = {
    {0x1c, 0xba, 0x05, 0x2f, 0x7a, 0x07, 0x3b, 0x9c,
     0x31, 0x97, 0xd5, 0x49, 0xcf, 0x68, 0xd3, 0x34,
     0x71, 0x36, 0x46, 0x3a, 0x47, 0x1e, 0x09, 0xa4,
     0xf6, 0x68, 0x3a, 0xab, 0xa4, 0xfd, 0x22, 0x1d},
    {0x92, 0x70, 0xcb, 0xe0, 0x86, 0x09, 0x1d, 0xbb,
     0x57, 0x5c, 0xc0, 0xb7, 0xa4, 0x1f, 0x08, 0x02,
     0xd6, 0x7b, 0x1f, 0x3a, 0x77, 0xb6, 0x6b, 0x6a,
     0x1e, 0xcd, 0x93, 0xf2, 0x59, 0x32, 0xe5, 0x64},
    {0xe2, 0xc1, 0x5f, 0x45, 0x4f, 0x8d, 0x4a, 0x6a,
     0x99, 0x73, 0xf0, 0x4f, 0x25, 0xa0, 0x76, 0xb8,
     0x94, 0x51, 0x10, 0x27, 0xe2, 0x73, 0x60, 0xf2,
     0x7b, 0x5d, 0xa2, 0xdd, 0x9f, 0x26, 0xd0, 0x7c}
};


static unsigned char test_sm[] = {
    0x28, 0x24, 0x78, 0xcc, 0xf8, 0x47, 0x56, 0xa1,
    0x4b, 0x71, 0x55, 0xc9, 0x82, 0x12, 0x85, 0x94,
    0xe2, 0x46, 0xdd, 0xc9, 0x7b, 0x49, 0x19, 0x34,
    0x45, 0x90, 0x38, 0x40, 0x06, 0x33, 0x21, 0x00,
    0xf2, 0x91, 0x2d, 0xd5, 0xa1, 0xb9, 0xe2, 0xe6,
    0x02, 0x6c, 0xd5, 0x50, 0x61, 0xbf, 0x8b, 0x35,
    0x59, 0xd1, 0x69, 0x87, 0x79, 0x92, 0x0f, 0x37,
    0x31, 0x04, 0x23, 0xac, 0xed, 0x31, 0x18, 0x09,
    0x68, 0x74, 0x74, 0x70, 0x73, 0x3a, 0x2f, 0x2f,
    0x65, 0x70, 0x72, 0x69, 0x6e, 0x74, 0x2e, 0x69,
    0x61, 0x63, 0x72, 0x2e, 0x6f, 0x72, 0x67, 0x2f,
    0x32, 0x30, 0x31, 0x38, 0x2f, 0x30, 0x36, 0x38,
    0x2e, 0x70, 0x64, 0x66, 0x00
};

int
main(void)
{
    size_t i,j,len;

    assert(strcmp(crypto_musig_primitive(), "musig_ristretto255") == 0);
    assert(crypto_musig_bytes() > 0U);
    assert(crypto_musig_seedbytes() > 0U);
    assert(crypto_musig_publickeybytes() > 0U);
    assert(crypto_musig_secretkeybytes() > 0U);
    assert(crypto_musig_rbytes() > 0U);
    assert(crypto_musig_kstbytes() > 0U);
    assert(crypto_musig_messagebytes_max() > 0U);
    assert(crypto_musig_statebytes() == sizeof(crypto_musig_state));

    for (len=0; len<=sizeof m; len += (sizeof m / 0x10))
    {
        // Random message to be signed
        randombytes_buf(m, len);

        // Initialize states and generate keypairs
        for (i=0; i<NSIGNERS; i++) {
            crypto_musig_init(&st[i]);
            if (crypto_musig_keypair(pk[i], sk[i])) {
                printf("crypto_musig_keypair failure\n");
                return -1;
            }
        }

        // Add public keys
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (crypto_musig_add(&st[i], pk[j])) {
                    printf("crypto_musig_add failure\n");
                    return -1;
                }
            }
        }

        // Generate aggregate public key
        for (i=0; i<NSIGNERS; i++) {
            if (crypto_musig_key(&st[i], mk[i])) {
                printf("crypto_musig_key failure\n");
                return -1;
            }
        }

        // Check that everyone computed the same aggregate key
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (sodium_memcmp(mk[i], mk[j],
                            crypto_musig_publickeybytes())) {
                    printf("crypto_musig_key mismatch\n");
                    return -1;
                }
            }
        }

        // Begin the signature protocol by generating R and T
        for (i=0; i<NSIGNERS; i++) {
            crypto_musig_begin(&st[i], r[i], t[i]);
        }

        // Commit by sharing T
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (crypto_musig_commit(&st[i], pk[j], t[j])) {
                        printf("crypto_musig_commit failure\n");
                        return -1;
                }
            }
        }

        // Share R and check everyone else's
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (crypto_musig_check(&st[i], pk[j], r[j])) {
                        printf("crypto_musig_check failure\n");
                        return -1;
                }
            }
        }

        // Produce own partial signature
        for (i=0; i<NSIGNERS; i++) {
            if (crypto_musig_sign(&st[i], s[i], sk[i], m, len)) {
                printf("crypto_musig_sign failure\n");
                return -1;
            }
        }

        // Add everyone else's partial signature
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (crypto_musig_update(&st[i], pk[j], s[j])) {
                    printf("crypto_musig_update failure\n");
                    return -1;
                }
            }
        }

        // Finally, compute aggregate signature
        for (i=0; i<NSIGNERS; i++) {
            if (crypto_musig_final_detached(&st[i], sg[i], m, len)) {
                printf("crypto_musig_final_detached failure\n");
                return -1;
            }
        }

        // Check that everyone computed the same aggregate signature
        for (i=0; i<NSIGNERS; i++) {
            for (j=0; j<NSIGNERS; j++) {
                if (i != j) {
                    if (sodium_memcmp(sg[i], sg[j], crypto_musig_bytes())) {
                        printf("crypto_musig signature mismatch\n");
                        return -1;
                    }
                }
            }
        }

        // Since they are all the same, verify the first one
        if (crypto_musig_verify_detached(sg[0], m, len, mk[0])) {
            printf("crypto_musig_verify_detached failure\n");
            return -1;
        }

        // Test correct operation of embedded signature functions
        if (crypto_musig_final(&st[0], sm, &sm_len, m, len)) {
            printf("crypto_musig_final failure\n");
            return -1;
        }
        if (sm_len != len + crypto_musig_bytes()) {
            printf("crypto_musig_final message length mismatch\n");
        }
        if (sodium_memcmp(m, sm + crypto_musig_bytes(), len)) {
            printf("crypto_musig_final message mismatch\n");
        }
        if (sodium_memcmp(sm, sg[0], crypto_musig_bytes())) {
            printf("crypto_musig_final signature mismatch\n");
        }
        if (crypto_musig_verify(m, &m_len, sm, sm_len, mk[0])) {
            printf("crypto_musig_verify failure\n");
            return -1;
        }

        for (i=0; i<NSIGNERS; i++) {
            crypto_musig_cleanup(&st[i]);
        }
    }

    // Verify precomputed 3-party test signature
    crypto_musig_init(&st[0]);
    if (crypto_musig_add(&st[0], test_pk[0]) ||
            crypto_musig_add(&st[0], test_pk[1]) ||
            crypto_musig_add(&st[0], test_pk[2]) ||
            crypto_musig_key(&st[0], mk[0]) ||
            crypto_musig_verify(m, &m_len, test_sm,
                sizeof test_sm, mk[0])) {
        printf("crypto_musig: test signature verification failure\n");
        return -1;
    }
    printf("%.*s\n", m_len, m);
    crypto_musig_cleanup(&st[0]);

    return 0;
}
