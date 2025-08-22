#ifdef __aarch64__

#include "../scalarmult_curve25519.h"

extern int curve25519_x25519_byte (unsigned char res[static 32], const unsigned char scalar[static 32],const unsigned char point[static 32]);
extern int curve25519_x25519base_byte(unsigned char res[static 32],const unsigned char scalar[static 32]);

struct crypto_scalarmult_curve25519_implementation
crypto_scalarmult_curve25519_aarch64_implementation = {
    .mult =  curve25519_x25519_byte,
    .mult_base = curve25519_x25519base_byte
};

#endif
