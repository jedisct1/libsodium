
#define TEST_NAME "vrf"
#include "cmptest.h"

typedef struct TestData_ {
    const char seed[2 * 32 + 1];
} TestData;
/*
 * Test data taken from https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-vrf-09#appendix-A.4
 * which contains the seeds. The expected values for the pk, proof and output are in vrf.exp
 */
static const TestData test_data[] = {
    {"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"},
    {"4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb"},
    {"c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7"},
};

static const unsigned char messages[3][2] = {{0x00}, {0x72}, {0xaf, 0x82}};

int main(void)
{
    unsigned char *seed;
    unsigned char sk[64];
    unsigned char pk[32];
    unsigned char proof[80];
    unsigned char output[64];
    unsigned int i;

    char pk_hex[32 * 2 + 1];
    char proof_hex[80 * 2 + 1];
    char output_hex[64 * 2 + 1];

    seed            = (unsigned char *) sodium_malloc(crypto_vrf_rfc9381_SEEDBYTES);

    assert(crypto_vrf_rfc9381_SECRETKEYBYTES == 64);
    assert(crypto_vrf_rfc9381_PUBLICKEYBYTES == 32);
    assert(crypto_vrf_rfc9381_SEEDBYTES == 32);
    assert(crypto_vrf_rfc9381_BYTES == 80);
    assert(crypto_vrf_rfc9381_OUTPUTBYTES == 64);

    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
        sodium_hex2bin(seed, 32,
                       test_data[i].seed, (size_t) -1U, NULL, NULL, NULL);

        crypto_vrf_rfc9381_seed_keypair(pk, sk, seed);
        printf("%s\n", sodium_bin2hex(pk_hex, sizeof pk_hex, pk, sizeof pk));

        if (crypto_vrf_rfc9381_prove(proof, messages[i], i, sk) != 0){
            printf("crypto_vrf_prove() error: [%u]\n", i);
        }
        printf("%s\n", sodium_bin2hex(proof_hex, sizeof proof_hex, proof, sizeof proof));

        if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i) != 0){
            printf("verify error: [%u]\n", i);
        }
        printf("%s\n", sodium_bin2hex(output_hex, sizeof output_hex, output, sizeof output));

        proof[0] ^= 0x01;
        if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i) == 0){
            printf("verify succeeded with bad gamma: [%u]\n", i);
        }
        proof[0] ^= 0x01;
        proof[32] ^= 0x01;
        if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i) == 0){
            printf("verify succeeded with bad c value: [%u]\n", i);
        }
        proof[32] ^= 0x01;
        proof[48] ^= 0x01;
        if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i) == 0){
            printf("verify succeeded with bad s value: [%u]\n", i);
        }
        proof[48] ^= 0x01;
        proof[79] ^= 0x80;
        if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i) == 0){
            printf("verify succeeded with bad s value (high-order-bit flipped): [%u]\n", i);
        }
        proof[79] ^= 0x80;

        if (i > 0) {
            if (crypto_vrf_rfc9381_verify(output, pk, proof, messages[i], i-1) == 0){
                printf("verify succeeded with truncated message: [%u]\n", i);
            }
        }

        if (crypto_vrf_rfc9381_proof_to_hash(output, proof) != 0){
            printf("crypto_vrf_proof_to_hash() error: [%u]\n", i);
        }
        printf("%s\n\n", sodium_bin2hex(output_hex, sizeof output_hex, output, sizeof output));
    }
    return 0;
}
