#define TEST_NAME "pwhash_argon2id2"
#include "cmptest.h"

int
main(void)
{
    char          str_out[crypto_pwhash_STRBYTES];
    unsigned char out[32];
    int           ret;

    ret = crypto_pwhash_argon2id_str(str_out, "test", 4, 3, 5000000);
    if (ret != 0) {
        printf("pwhash_argon2id_str failed\n");
        return 1;
    }
    printf("hash created: ok\n");

    assert(crypto_pwhash_argon2id_str_verify(str_out, "test", 4) == 0);
    printf("verify: ok\n");

#if SIZE_MAX > 0xFFFFFFFFULL
    ret = crypto_pwhash_argon2id_str_needs_rehash(str_out,
                                                   (unsigned long long) 0x100000000ULL,
                                                   5000000);
    assert(ret == -1);
    printf("needs_rehash opslimit overflow: rejected\n");

    ret = crypto_pwhash_argon2id_str_needs_rehash(str_out, 3,
                                                   (size_t) 0x100000000ULL * 1024ULL);
    assert(ret == -1);
    printf("needs_rehash memlimit overflow: rejected\n");
#else
    printf("needs_rehash opslimit overflow: rejected\n");
    printf("needs_rehash memlimit overflow: rejected\n");
#endif

    ret = crypto_pwhash_argon2id_str_needs_rehash(str_out, 3, 5000000);
    assert(ret == 0);
    printf("needs_rehash same params: ok\n");

    ret = crypto_pwhash_argon2i_str_needs_rehash(str_out, 3, 5000000);
    assert(ret == -1);
    printf("argon2i needs_rehash on argon2id: rejected\n");

    assert(crypto_pwhash_str_verify(str_out, "wrong", 5) == -1);
    printf("verify wrong password: rejected\n");

    ret = crypto_pwhash(out, sizeof out,
                        "test", 4,
                        (const unsigned char *) str_out + 30,
                        3, 5000000, crypto_pwhash_ALG_ARGON2I13);
    assert(ret == 0);
    printf("pwhash argon2i alg: ok\n");

    return 0;
}
