#include <stdio.h>
#include <sodium.h>
#include "vectors/aes256gcm_vectors.h"

#define AEAD_KEYBYTES  crypto_aead_aes256gcm_KEYBYTES
#define AEAD_NPUBBYTES crypto_aead_aes256gcm_NPUBBYTES
#define AEAD_ABYTES    crypto_aead_aes256gcm_ABYTES
#define AEAD_ENCRYPT   crypto_aead_aes256gcm_encrypt
#define AEAD_DECRYPT   crypto_aead_aes256gcm_decrypt
#define TEST_NAME      "aes256gcm"

#include "aead_tests.h"

int
main(void)
{
    if (sodium_init() < 0) {
        return 1;
    }
    if (crypto_aead_aes256gcm_is_available() == 0) {
        printf("[%s] not available on this CPU, skipping\n",
               TEST_NAME);
        return 0;
    }
    return run_aead_tests();
}
