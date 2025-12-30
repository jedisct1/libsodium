#include "crypto_xof_turboshake256.h"
#include "private/common.h"
#include "ref/turboshake256_ref.h"

size_t
crypto_xof_turboshake256_blockbytes(void)
{
    return crypto_xof_turboshake256_BLOCKBYTES;
}

size_t
crypto_xof_turboshake256_statebytes(void)
{
    return crypto_xof_turboshake256_STATEBYTES;
}

unsigned char
crypto_xof_turboshake256_domain_standard(void)
{
    return crypto_xof_turboshake256_DOMAIN_STANDARD;
}

int
crypto_xof_turboshake256(unsigned char *out, size_t outlen, const unsigned char *in,
                         unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_xof_turboshake256_state) >= sizeof(turboshake256_state_internal));

    return turboshake256_ref(out, outlen, in, (size_t) inlen);
}

int
crypto_xof_turboshake256_init(crypto_xof_turboshake256_state *state)
{
    turboshake256_state_internal *st = (turboshake256_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_turboshake256_state) >= sizeof(turboshake256_state_internal));

    return turboshake256_ref_init(st);
}

int
crypto_xof_turboshake256_init_with_domain(crypto_xof_turboshake256_state *state,
                                          unsigned char                   domain)
{
    turboshake256_state_internal *st = (turboshake256_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_turboshake256_state) >= sizeof(turboshake256_state_internal));

    return turboshake256_ref_init_with_domain(st, domain);
}

int
crypto_xof_turboshake256_update(crypto_xof_turboshake256_state *state,
                                const unsigned char            *in,
                                unsigned long long              inlen)
{
    turboshake256_state_internal *st = (turboshake256_state_internal *) (void *) state;

    return turboshake256_ref_update(st, in, (size_t) inlen);
}

int
crypto_xof_turboshake256_squeeze(crypto_xof_turboshake256_state *state, unsigned char *out,
                                 size_t outlen)
{
    turboshake256_state_internal *st = (turboshake256_state_internal *) (void *) state;

    return turboshake256_ref_squeeze(st, out, outlen);
}
