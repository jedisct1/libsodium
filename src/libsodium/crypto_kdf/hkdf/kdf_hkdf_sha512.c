#include <errno.h>
#include <string.h>

#include "crypto_auth_hmacsha512.h"
#include "crypto_kdf.h"
#include "crypto_kdf_hkdf_sha512.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_kdf_hkdf_sha512_extract(
    unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES],
    const unsigned char *salt, size_t salt_len, const unsigned char *ikm,
    size_t ikm_len)
{
    crypto_auth_hmacsha512_state st;

    crypto_auth_hmacsha512_init(&st, salt, salt_len);
    crypto_auth_hmacsha512_update(&st, ikm, ikm_len);
    crypto_auth_hmacsha512_final(&st, prk);
    sodium_memzero(&st, sizeof st);

    return 0;
}

void
crypto_kdf_hkdf_sha512_keygen(unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
{
    randombytes_buf(prk, crypto_kdf_hkdf_sha512_KEYBYTES);
}

int
crypto_kdf_hkdf_sha512_expand(unsigned char *out, size_t out_len,
                              const char *ctx, size_t ctx_len,
                              const unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
{
    crypto_auth_hmacsha512_state st;
    unsigned char                tmp[crypto_auth_hmacsha512_BYTES];
    size_t                       i;
    size_t                       left;
    unsigned char                counter = 1U;

    if (out_len > crypto_kdf_hkdf_sha512_BYTES_MAX) {
        errno = EINVAL;
        return -1;
    }
    for (i = (size_t) 0U; i + crypto_auth_hmacsha512_BYTES <= out_len;
         i += crypto_auth_hmacsha512_BYTES) {
        crypto_auth_hmacsha512_init(&st, prk, crypto_kdf_hkdf_sha512_KEYBYTES);
        if (i != (size_t) 0U) {
            crypto_auth_hmacsha512_update(
                &st, &out[i - crypto_auth_hmacsha512_BYTES],
                crypto_auth_hmacsha512_BYTES);
        }
        crypto_auth_hmacsha512_update(&st,
                                      (const unsigned char *) ctx, ctx_len);
        crypto_auth_hmacsha512_update(&st, &counter, (size_t) 1U);
        crypto_auth_hmacsha512_final(&st, &out[i]);
        counter++;
    }
    if ((left = out_len & (crypto_auth_hmacsha512_BYTES - 1U)) != (size_t) 0U) {
        crypto_auth_hmacsha512_init(&st, prk, crypto_kdf_hkdf_sha512_KEYBYTES);
        if (i != (size_t) 0U) {
            crypto_auth_hmacsha512_update(
                &st, &out[i - crypto_auth_hmacsha512_BYTES],
                crypto_auth_hmacsha512_BYTES);
        }
        crypto_auth_hmacsha512_update(&st,
                                      (const unsigned char *) ctx, ctx_len);
        crypto_auth_hmacsha512_update(&st, &counter, (size_t) 1U);
        crypto_auth_hmacsha512_final(&st, tmp);
        memcpy(&out[i], tmp, left);
        sodium_memzero(tmp, sizeof tmp);
    }
    sodium_memzero(&st, sizeof st);

    return 0;
}

size_t
crypto_kdf_hkdf_sha512_keybytes(void)
{
    return crypto_kdf_hkdf_sha512_KEYBYTES;
}

size_t
crypto_kdf_hkdf_sha512_bytes_min(void)
{
    return crypto_kdf_hkdf_sha512_BYTES_MIN;
}

size_t
crypto_kdf_hkdf_sha512_bytes_max(void)
{
    return crypto_kdf_hkdf_sha512_BYTES_MAX;
}
