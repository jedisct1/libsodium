#include "crypto_auth_hmacsha256.h"
#include "randombytes.h"

size_t
crypto_auth_hmacsha256_bytes(void)
{
    return crypto_auth_hmacsha256_BYTES;
}

size_t
crypto_auth_hmacsha256_keybytes(void)
{
    return crypto_auth_hmacsha256_KEYBYTES;
}

size_t
crypto_auth_hmacsha256_statebytes(void)
{
    return sizeof(crypto_auth_hmacsha256_state);
}

void
crypto_auth_hmacsha256_keygen(unsigned char k[crypto_auth_hmacsha256_KEYBYTES])
{
    randombytes_buf(k, crypto_auth_hmacsha256_KEYBYTES);
}
