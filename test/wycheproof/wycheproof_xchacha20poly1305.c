#include <sodium.h>
#include "vectors/xchacha20_poly1305_vectors.h"

#define AEAD_KEYBYTES  crypto_aead_xchacha20poly1305_ietf_KEYBYTES
#define AEAD_NPUBBYTES crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
#define AEAD_ABYTES    crypto_aead_xchacha20poly1305_ietf_ABYTES
#define AEAD_ENCRYPT   crypto_aead_xchacha20poly1305_ietf_encrypt
#define AEAD_DECRYPT   crypto_aead_xchacha20poly1305_ietf_decrypt
#define TEST_NAME      "xchacha20poly1305"

#include "aead_tests.h"

int
main(void)
{
    if (sodium_init() < 0) {
        return 1;
    }
    return run_aead_tests();
}
