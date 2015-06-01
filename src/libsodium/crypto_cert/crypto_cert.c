
#include "crypto_cert.h"

#define crypto_cert_HEADER_BYTES (sizeof(uint8_t) /*version*/ \
                                 +sizeof(uint8_t) /*flags*/ \
                                 +sizeof(uint32_t) /*valid from*/ \
                                 +sizeof(uint32_t) /*valid until*/ \
)

static int
crypto_cert_swapl(uint32_t n)
{
#if defined(NATIVE_LITTLE_ENDIAN)
    unsigned char *np = (unsigned char*)&n;
    return
        ((uint32_t)np[0] << 24) |
        ((uint32_t)np[1] << 16) |
        ((uint32_t)np[2] << 8) |
        (uint32_t)np[3];
#endif
    return n;
}

int
crypto_cert(unsigned char *out, uint8_t flags, time_t valid_from,
            time_t valid_until, const unsigned char *ad,
            unsigned long long adlen, const unsigned char *pk,
            const unsigned char *s, const unsigned char *k)
{
    //build cert
    unsigned char *pos = out;
    const uint8_t version = crypto_cert_VERSION;

    memcpy(pos, &version, sizeof(version));
    pos += sizeof(version);

    memcpy(pos, &flags, sizeof(flags));
    pos += sizeof(flags);

    const uint32_t from = crypto_cert_swapl((uint32_t)valid_from);
    memcpy(pos, &from, sizeof(from));
    pos += sizeof(from);

    const uint32_t until = crypto_cert_swapl((uint32_t)valid_until);
    memcpy(pos, &until, sizeof(until));
    pos += sizeof(until);

    memcpy(pos, pk, crypto_sign_PUBLICKEYBYTES);
    pos += crypto_sign_PUBLICKEYBYTES;

    memcpy(pos, s, crypto_sign_PUBLICKEYBYTES);
    pos += crypto_sign_PUBLICKEYBYTES;

    //hash cert & additional data
    unsigned char hash[crypto_generichash_BYTES];
    crypto_generichash_state state;
    crypto_generichash_init(&state, NULL, 0, sizeof(hash));
    crypto_generichash_update(&state, out, pos - out);
    crypto_generichash_update(&state, ad, adlen);
    crypto_generichash_final(&state, hash, sizeof(hash));

    //sign hash & append signature to cert
    crypto_sign_detached(pos, NULL, hash, sizeof(hash), k);
    return crypto_sign_verify_detached(pos, hash, sizeof(hash), s);
}

int
crypto_cert_verify(const unsigned char *c, const unsigned char *ad,
                   unsigned long long adlen)
{
    //verify version
    if(crypto_cert_version(c) != crypto_cert_VERSION)
        return -1;

    //verify validity period
    time_t now = time(NULL);
    if(crypto_cert_valid_from(c) > now || crypto_cert_valid_until(c) < now)
    {
        return -1;
    }

    //hash cert & additional data
    unsigned char hash[crypto_generichash_BYTES];
    crypto_generichash_state state;
    crypto_generichash_init(&state, NULL, 0, sizeof(hash));
    crypto_generichash_update(&state, c, crypto_cert_BYTES -
        crypto_sign_BYTES);
    crypto_generichash_update(&state, ad, adlen);
    crypto_generichash_final(&state, hash, sizeof(hash));

    //verify signature
    unsigned char s[crypto_sign_PUBLICKEYBYTES];
    crypto_cert_signer(s, c);
    return crypto_sign_verify_detached(c + crypto_cert_BYTES -
        crypto_sign_BYTES, hash, sizeof(hash), s);
}

void
crypto_cert_pk(unsigned char *out, const unsigned char *c)
{
    memcpy(out, c + crypto_cert_HEADER_BYTES, crypto_sign_PUBLICKEYBYTES);
}

void
crypto_cert_signer(unsigned char *out, const unsigned char *c)
{
    memcpy(out, c + crypto_cert_HEADER_BYTES + crypto_sign_PUBLICKEYBYTES,
        crypto_sign_PUBLICKEYBYTES);
}

void
crypto_cert_signature(unsigned char *out, const unsigned char *c)
{
    memcpy(out, c + crypto_cert_HEADER_BYTES + crypto_sign_PUBLICKEYBYTES * 2,
        crypto_sign_BYTES);
}

time_t
crypto_cert_valid_from(const unsigned char *c)
{
    uint32_t from;
    c += sizeof(uint8_t) /*version*/ + sizeof(uint8_t) /*flags*/;
    memcpy(&from, c, sizeof(from));
    return crypto_cert_swapl(from);
}

time_t
crypto_cert_valid_until(const unsigned char *c)
{
    uint32_t until;
    c += sizeof(uint8_t) /*version*/ + sizeof(uint8_t) /*flags*/
        + sizeof(uint32_t) /*from*/;
    memcpy(&until, c, sizeof(until));
    return crypto_cert_swapl(until);
}

uint8_t
crypto_cert_version(const unsigned char *c)
{
    return (uint8_t)c[0];
}

uint8_t
crypto_cert_flags(const unsigned char *c) {
    return (uint8_t)c[1];
}
