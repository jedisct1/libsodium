#include <stdio.h>
#include <string.h>

#define TEST_NAME "pwhash"
#include "cmptest.h"

#define OUT_LEN 128
#define MEMLIMIT 10000000
#define OPSLIMIT 1000000

int main(void)
{
    char           str_out[crypto_pwhash_scryptxsalsa208sha256_STRBYTES];
    unsigned char  out[OUT_LEN];
    char           out_hex[OUT_LEN * 2 + 1];
    const char    *salt = "0a2c4e63%2AxC+E7";
    const char    *passwd = "Correct Horse Battery Staple";

    if (crypto_pwhash_scryptxsalsa208sha256(out, sizeof out,
                                            passwd, strlen(passwd),
                                            (const unsigned char *) salt,
                                            MEMLIMIT, OPSLIMIT) != 0) {
        printf("pwhash failure\n");
    }
    sodium_bin2hex(out_hex, sizeof out_hex, out, sizeof out);
    printf("out_hex: [%s]\n", out_hex);
    if (crypto_pwhash_scryptxsalsa208sha256_str(str_out, passwd, strlen(passwd),
                                                MEMLIMIT, OPSLIMIT) != 0) {
        printf("pwhash_str failure\n");
    }
    puts(str_out);
    if (crypto_pwhash_scryptxsalsa208sha256_str_verify(str_out, passwd,
                                                       strlen(passwd)) != 0) {
        printf("pwhash_str_verify failure\n");
    }
    printf("OK\n");

    return 0;
}
