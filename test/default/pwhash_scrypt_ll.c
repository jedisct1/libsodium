#include <stdio.h>
#include <string.h>

#define TEST_NAME "pwhash_scrypt_ll"
#include "cmptest.h"

/* Tarsnap test vectors, see: https://www.tarsnap.com/scrypt/scrypt.pdf */
unsigned long olenGlobal = 64ul;

uint8_t password1[] = "\0";
uint8_t salt1[] = "\0";
uint64_t N1 = 16ul;
uint32_t r1 = 1ul;
uint32_t p1 = 1ul;

uint8_t password2[] = "password\0";
uint8_t salt2[] = "NaCl\0";
uint64_t N2 = 1024ul;
uint32_t r2 = 8ul;
uint32_t p2 = 16ul;

uint8_t password3[] = "pleaseletmein\0";
uint8_t salt3[] = "SodiumChloride\0";
uint64_t N3 = 16384ul;
uint32_t r3 = 8ul;
uint32_t p3 = 1ul;

uint8_t password4[] = "pleaseletmein\0";
uint8_t salt4[] = "SodiumChloride\0";
size_t N4 = 1048576ul;
size_t r4 = 8ul;
size_t p4 = 1ul;

void test_vector(uint8_t * password, size_t passwordLength, uint8_t * salt, size_t saltLenght, uint64_t N, uint32_t r, uint32_t p, size_t olen)
{
    uint8_t data[olen];
    int ret = crypto_pwhash_scryptsalsa208sha256_ll(password, passwordLength, salt, saltLenght, N, r, p, data, olen);
    if(ret != 0)
    {
        printf("crypto_scrypt_compat failed!");
    } 
    
    // Print header
    printf("scrypt(“");
    printf(password);
    printf("”, “");
    printf(salt);
    printf("”, %u, %u, %u, %u) =\n", N, r, p, olen);

    // Print test result
    int lineitems = 0;
    const int lineitemsLimit = 15;
    for(int i = 0; i < olen; ++i)
    {
        printf("%02x", data[i]);
        printf((lineitems < lineitemsLimit)? " " : "\n");
        lineitems = (lineitems < lineitemsLimit)? lineitems+1 : 0;
    }
}

int main(void)
{
   test_vector(password1, strlen(password1), salt1, strlen(salt1), N1, r1, p1, olenGlobal);
   test_vector(password2, strlen(password2), salt2, strlen(salt2), N2, r2, p2, olenGlobal);
   test_vector(password3, strlen(password3), salt3, strlen(salt3), N3, r3, p3, olenGlobal);
   test_vector(password4, strlen(password4), salt4, strlen(salt4), N4, r4, p4, olenGlobal);

   return 0;
}
