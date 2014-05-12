#include <stdio.h>
#include <string.h>

#define TEST_NAME "pwhash"
#include "cmptest.h"

#define OUT_LEN 128
#define OPSLIMIT 1000000
#define MEMLIMIT 10000000

int main(void)
{
    char           str_out[crypto_pwhash_scryptxsalsa208sha256_STRBYTES];
    char           str_out2[crypto_pwhash_scryptxsalsa208sha256_STRBYTES];
    unsigned char  out[OUT_LEN];
    char           out_hex[OUT_LEN * 2 + 1];
    const char    *salt = "[<~A 32-bytes salt for scrypt~>]";
    const char    *passwd = "Correct Horse Battery Staple";
    size_t         i;

    if (crypto_pwhash_scryptxsalsa208sha256(out, sizeof out,
                                            passwd, strlen(passwd),
                                            (const unsigned char *) salt,
                                            OPSLIMIT, MEMLIMIT) != 0) {
        printf("pwhash failure\n");
    }
    sodium_bin2hex(out_hex, sizeof out_hex, out, sizeof out);
    printf("out_hex: [%s]\n", out_hex);
    if (crypto_pwhash_scryptxsalsa208sha256_str(str_out, passwd, strlen(passwd),
                                                OPSLIMIT, MEMLIMIT) != 0) {
        printf("pwhash_str failure\n");
    }
    if (crypto_pwhash_scryptxsalsa208sha256_str(str_out2, passwd, strlen(passwd),
                                                OPSLIMIT, MEMLIMIT) != 0) {
        printf("pwhash_str(2) failure\n");
    }
    if (strcmp(str_out, str_out2) == 0) {
        printf("pwhash_str doesn't generate different salts\n");
    }
    if (crypto_pwhash_scryptxsalsa208sha256_str_verify(str_out, passwd,
                                                       strlen(passwd)) != 0) {
        printf("pwhash_str_verify failure\n");
    }
    if (crypto_pwhash_scryptxsalsa208sha256_str_verify(str_out, passwd,
                                                       strlen(passwd)) != 0) {
        printf("pwhash_str_verify failure\n");
    }
    for (i = 14U; i < sizeof str_out; i++) {
        str_out[i]++;
        if (crypto_pwhash_scryptxsalsa208sha256_str_verify(str_out, passwd,
                                                           strlen(passwd)) == 0) {
            printf("pwhash_str_verify(2) failure\n");
        }
        str_out[i]--;
    }
    printf("OK\n");

    return 0;
}
