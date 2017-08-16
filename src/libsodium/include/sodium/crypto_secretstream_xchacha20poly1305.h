#ifndef crypto_secretstream_xchacha20poly1305_H
#define crypto_secretstream_xchacha20poly1305_H

#include <stddef.h>

#include "crypto_aead_xchacha20poly1305.h"
#include "crypto_stream_chacha20.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_secretstream_xchacha20poly1305_ABYTES \
    (1U + crypto_aead_xchacha20poly1305_ietf_ABYTES)

#define crypto_secretstream_xchacha20poly1305_INITBYTES \
    crypto_aead_xchacha20poly1305_ietf_NPUBBYTES

#define crypto_secretstream_xchacha20poly1305_KEYBYTES \
    crypto_aead_xchacha20poly1305_ietf_KEYBYTES

#define crypto_secretstream_xchacha20poly1305_MESSAGESBYTES_MAX \
    ((1ULL << 32) - 2ULL * 64ULL)

#define crypto_secretstream_xchacha20poly1305_TAG_MESSAGE 0x00
#define crypto_secretstream_xchacha20poly1305_TAG_PUSH    0x01
#define crypto_secretstream_xchacha20poly1305_TAG_REKEY   0x02

#define crypto_secretstream_xchacha20poly1305_TAG_FINAL \
    (crypto_secretstream_xchacha20poly1305_TAG_PUSH | \
     crypto_secretstream_xchacha20poly1305_TAG_REKEY)

typedef struct crypto_secretstream_xchacha20poly1305_state {
    unsigned char k[crypto_stream_chacha20_ietf_KEYBYTES];
    unsigned char nonce[crypto_stream_chacha20_ietf_NONCEBYTES];
    unsigned char _pad[8];
} crypto_secretstream_xchacha20poly1305_state;

SODIUM_EXPORT
size_t crypto_secretstream_xchacha20poly1305_statebytes(void);

SODIUM_EXPORT
void crypto_secretstream_xchacha20poly1305_keygen
   (unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

SODIUM_EXPORT
int crypto_secretstream_xchacha20poly1305_init_push
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned char out[crypto_secretstream_xchacha20poly1305_INITBYTES],
    const unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

SODIUM_EXPORT
int crypto_secretstream_xchacha20poly1305_push
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned char *out, unsigned long long *outlen_p,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen, unsigned char tag);

SODIUM_EXPORT
int crypto_secretstream_xchacha20poly1305_init_pull
   (crypto_secretstream_xchacha20poly1305_state *state,
    const unsigned char in[crypto_secretstream_xchacha20poly1305_INITBYTES],
    const unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

SODIUM_EXPORT
int crypto_secretstream_xchacha20poly1305_pull
   (crypto_secretstream_xchacha20poly1305_state *state,
    unsigned char *m, unsigned long long *mlen_p, unsigned char *tag_p,
    const unsigned char *in, unsigned long long inlen,
    const unsigned char *ad, unsigned long long adlen);

SODIUM_EXPORT
void crypto_secretstream_xchacha20poly1305_rekey
    (crypto_secretstream_xchacha20poly1305_state *state);

#ifdef __cplusplus
}
#endif

#endif
