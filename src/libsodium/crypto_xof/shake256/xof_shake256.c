#include "crypto_xof_shake256.h"
#include "private/common.h"
#include "ref/shake256_ref.h"

size_t
crypto_xof_shake256_blockbytes(void)
{
    return crypto_xof_shake256_BLOCKBYTES;
}

size_t
crypto_xof_shake256_statebytes(void)
{
    return crypto_xof_shake256_STATEBYTES;
}

unsigned char
crypto_xof_shake256_domain_standard(void)
{
    return crypto_xof_shake256_DOMAIN_STANDARD;
}

int
crypto_xof_shake256(unsigned char *out, size_t outlen, const unsigned char *in,
                    unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_xof_shake256_state) >= sizeof(shake256_state_internal));

    return shake256_ref(out, outlen, in, (size_t) inlen);
}

int
crypto_xof_shake256_init(crypto_xof_shake256_state *state)
{
    shake256_state_internal *st = (shake256_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_shake256_state) >= sizeof(shake256_state_internal));

    return shake256_ref_init(st);
}

int
crypto_xof_shake256_init_with_domain(crypto_xof_shake256_state *state, unsigned char domain)
{
    shake256_state_internal *st = (shake256_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_shake256_state) >= sizeof(shake256_state_internal));

    return shake256_ref_init_with_domain(st, domain);
}

int
crypto_xof_shake256_update(crypto_xof_shake256_state *state,
                           const unsigned char       *in,
                           unsigned long long         inlen)
{
    shake256_state_internal *st = (shake256_state_internal *) (void *) state;

    return shake256_ref_update(st, in, (size_t) inlen);
}

int
crypto_xof_shake256_squeeze(crypto_xof_shake256_state *state, unsigned char *out, size_t outlen)
{
    shake256_state_internal *st = (shake256_state_internal *) (void *) state;

    return shake256_ref_squeeze(st, out, outlen);
}
