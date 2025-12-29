
#ifndef scalarmult_curve25519_H
#define scalarmult_curve25519_H

typedef struct crypto_scalarmult_curve25519_implementation {
    int (*mult)(unsigned char *q, const unsigned char *n,
                const unsigned char *p);
    int (*mult_base)(unsigned char *q, const unsigned char *n);
} crypto_scalarmult_curve25519_implementation;

#endif
