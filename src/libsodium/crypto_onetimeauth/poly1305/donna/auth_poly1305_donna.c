
#include "api.h"
#include "utils.h"

#include "crypto_onetimeauth_poly1305_donna.h"
#include "poly1305_donna.h"

#ifdef HAVE_TI_MODE
# include "poly1305_donna64.h"
#else
# include "poly1305_donna32.h"
#endif

static void
poly1305_update(poly1305_context *ctx, const unsigned char *m,
                unsigned long long bytes) {
        poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
        unsigned long long i;

        /* handle leftover */
        if (st->leftover) {
                unsigned long long want = (poly1305_block_size - st->leftover);
                if (want > bytes)
                        want = bytes;
                for (i = 0; i < want; i++)
                        st->buffer[st->leftover + i] = m[i];
                bytes -= want;
                m += want;
                st->leftover += want;
                if (st->leftover < poly1305_block_size)
                        return;
                poly1305_blocks(st, st->buffer, poly1305_block_size);
                st->leftover = 0;
        }

        /* process full blocks */
        if (bytes >= poly1305_block_size) {
                unsigned long long want = (bytes & ~(poly1305_block_size - 1));
                poly1305_blocks(st, m, want);
                m += want;
                bytes -= want;
        }

        /* store leftover */
        if (bytes) {
                for (i = 0; i < bytes; i++)
                        st->buffer[st->leftover + i] = m[i];
                st->leftover += bytes;
        }
}

int
crypto_onetimeauth(unsigned char *out, const unsigned char *m,
                   unsigned long long inlen, const unsigned char *key)
{
        poly1305_context ctx;
        poly1305_init(&ctx, key);
        poly1305_update(&ctx, m, inlen);
        poly1305_finish(&ctx, out);

    return 0;
}

const char *
crypto_onetimeauth_poly1305_implementation_name(void)
{
    return POLY1305_IMPLEMENTATION_NAME;
}

struct crypto_onetimeauth_poly1305_implementation
crypto_onetimeauth_poly1305_donna_implementation = {
    _SODIUM_C99(.implementation_name =) crypto_onetimeauth_poly1305_implementation_name,
    _SODIUM_C99(.onetimeauth =) crypto_onetimeauth,
    _SODIUM_C99(.onetimeauth_verify =) crypto_onetimeauth_verify
};
