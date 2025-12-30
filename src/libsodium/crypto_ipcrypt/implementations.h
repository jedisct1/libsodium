#ifndef ipcrypt_implementations_H
#define ipcrypt_implementations_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_ipcrypt.h"

typedef struct ipcrypt_implementation {
    void (*encrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
    void (*decrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
    void (*nd_encrypt)(uint8_t *out, const uint8_t *in, const uint8_t *t, const uint8_t *k);
    void (*nd_decrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
    void (*ndx_encrypt)(uint8_t *out, const uint8_t *in, const uint8_t *t, const uint8_t *k);
    void (*ndx_decrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
    void (*pfx_encrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
    void (*pfx_decrypt)(uint8_t *out, const uint8_t *in, const uint8_t *k);
} ipcrypt_implementation;

#endif
