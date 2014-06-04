#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TEST_NAME "pwhash_scrypt_ll"
#include "cmptest.h"

/* Tarsnap test vectors, see: https://www.tarsnap.com/scrypt/scrypt.pdf */

uint8_t password1[] = "";
uint8_t salt1[] = "";
uint64_t N1 = 16ul;
uint32_t r1 = 1ul;
uint32_t p1 = 1ul;

uint8_t password2[] = "password";
uint8_t salt2[] = "NaCl";
uint64_t N2 = 1024ul;
uint32_t r2 = 8ul;
uint32_t p2 = 16ul;

uint8_t password3[] = "pleaseletmein";
uint8_t salt3[] = "SodiumChloride";
uint64_t N3 = 16384ul;
uint32_t r3 = 8ul;
uint32_t p3 = 1ul;

uint8_t password4[] = "pleaseletmein";
uint8_t salt4[] = "SodiumChloride";
size_t N4 = 1048576ul;
size_t r4 = 8ul;
size_t p4 = 1ul;

void test_vector(uint8_t * password,  uint8_t * salt, uint64_t N, uint32_t r, uint32_t p)
{
    const size_t olen = 64;
    uint8_t data[64];
    size_t passwordLength = strlen((const char*) password);
    size_t saltLenght = strlen((const char*) salt);
    int lineitems = 0;
    const int lineitemsLimit = 15;

    int i = crypto_pwhash_scryptsalsa208sha256_ll(password, passwordLength, salt, saltLenght, N, r, p, data, olen);
    if(i != 0)
    {
        printf("crypto_scrypt_compat failed!");
    }
 
    // Print header
    printf("scrypt('");
    printf("%s", password);
    printf("', '");
    printf("%s", salt);
    printf("', %llu, %lu, %lu, %lu) =",
           (unsigned long long) N, (unsigned long) r, (unsigned long) p,
           (unsigned long) olen);
    printf("\n");

    // Print test result
    for(i = 0; i < olen; ++i)
    {
        printf("%02x", data[i]);
        printf((lineitems < lineitemsLimit)? " " : "\n");
        lineitems = (lineitems < lineitemsLimit)? lineitems+1 : 0;
    }
}

int main(void)
{
   test_vector(password1, salt1, N1, r1, p1);
   test_vector(password2, salt2, N2, r2, p2);
   test_vector(password3, salt3, N3, r3, p3);
   test_vector(password4, salt4, N4, r4, p4);

   return 0;
}
