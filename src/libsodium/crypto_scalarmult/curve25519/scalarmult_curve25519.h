
#ifndef scalarmult_poly1305_H
#define scalarmult_poly1305_H

typedef struct crypto_scalarmult_curve25519_implementation {
    int (*mult)(unsigned char *q, const unsigned char *n,
                const unsigned char *p, const int clamp);
    int (*mult_base)(unsigned char *q, const unsigned char *n, const int clamp);
} crypto_scalarmult_curve25519_implementation;

#endif
