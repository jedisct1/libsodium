/* aes-ctr.c               */
/* AES in CTR mode.        */

/* Hongjun Wu, January 2007*/


/* ------------------------------------------------------------------------- */

#include "api.h"
#include "aes256.h"

#include <string.h>

/* ------------------------------------------------------------------------- */
/* key setup for AES-256*/
static void
ECRYPT_keysetup(ECRYPT_ctx* ctx, const u8* key, u32 keysize, u32 ivsize)
{
    unsigned int w[Nk*(Nr+1)], temp;
    int i, j;

    (void) keysize;
    (void) ivsize;
    (void) sizeof(char[sizeof *ctx == crypto_stream_BEFORENMBYTES ? 1 : -1]);

    for( i = 0; i < Nk; i++ ) {
        w[i] =  key[(i << 2)];
        w[i] |= key[(i << 2)+1] << 8;
        w[i] |= key[(i << 2)+2] << 16;
        w[i] |= key[(i << 2)+3] << 24;
    }

    i = Nk;

    while( i < Nb*(Nr+1) ) {
        temp = w[i-1];

        temp = Sbox[ temp & 0xFF] <<  24 ^
            Sbox[(temp >> 8) & 0xFF]  ^
            (Sbox[(temp >> 16) & 0xFF] << 8 ) ^
            (Sbox[(temp >> 24) & 0xFF] << 16) ^
            Rcon[i/Nk];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        temp = Sbox[ temp & 0xFF] ^
            Sbox[(temp >> 8) & 0xFF] << 8 ^
            (Sbox[(temp >> 16) & 0xFF] << 16 ) ^
            (Sbox[(temp >> 24) & 0xFF] << 24);
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;

        temp = w[i-1];
        w[i] = w[i-Nk] ^ temp;
        i++;
    }

    for (i = 0; i <= Nr; i++) {
        for (j = 0; j < Nb; j++) {
            ctx->round_key[i][j] = w[(i<<2)+j];
        }
    }
}

/* ------------------------------------------------------------------------- */

static void
ECRYPT_ivsetup(ECRYPT_ctx* ctx, const u8* iv)
{
    (void) sizeof(char[(sizeof ctx->counter) == crypto_stream_NONCEBYTES ? 1 : -1]);
    memcpy(ctx->counter, iv, crypto_stream_NONCEBYTES);
}

/* ------------------------------------------------------------------------- */

static void
ECRYPT_process_bytes(int action, ECRYPT_ctx* ctx, const u8* input, u8* output,
                     u32 msglen)
{
    u8 keystream[16];
    u32 i;

    (void) action;
    memset(keystream, 0, sizeof keystream);
    partial_precompute_tworounds(ctx);

    for ( ; msglen >= 16; msglen -= 16, input += 16, output += 16) {
        aes256_enc_block(ctx->counter, keystream, ctx);

        ((u32*)output)[0] = ((u32*)input)[0] ^ ((u32*)keystream)[0] ^ ctx->round_key[Nr][0];
        ((u32*)output)[1] = ((u32*)input)[1] ^ ((u32*)keystream)[1] ^ ctx->round_key[Nr][1];
        ((u32*)output)[2] = ((u32*)input)[2] ^ ((u32*)keystream)[2] ^ ctx->round_key[Nr][2];
        ((u32*)output)[3] = ((u32*)input)[3] ^ ((u32*)keystream)[3] ^ ctx->round_key[Nr][3];

        ctx->counter[0]++;

        if ((ctx->counter[0] & 0xff)== 0) {
            partial_precompute_tworounds(ctx);
        }
    }

    if (msglen > 0) {
        aes256_enc_block(ctx->counter, keystream, ctx);
        ((u32*)keystream)[0] ^= ctx->round_key[Nr][0];
        ((u32*)keystream)[1] ^= ctx->round_key[Nr][1];
        ((u32*)keystream)[2] ^= ctx->round_key[Nr][2];
        ((u32*)keystream)[3] ^= ctx->round_key[Nr][3];

        for (i = 0; i < msglen; i ++) {
            output[i] = input[i] ^ keystream[i];
        }
    }
}

/* ------------------------------------------------------------------------- */

#include "ecrypt-sync.h"

int
crypto_stream_beforenm(unsigned char *c, const unsigned char *k)
{
    ECRYPT_ctx * const ctx = (ECRYPT_ctx *) c;

    ECRYPT_keysetup(ctx, k, crypto_stream_KEYBYTES * 8,
                    crypto_stream_NONCEBYTES * 8);
    return 0;
}

int
crypto_stream_afternm(unsigned char *outp, unsigned long long len,
                      const unsigned char *noncep, const unsigned char *c)
{
    ECRYPT_ctx * const ctx = (ECRYPT_ctx *) c;
    unsigned long long i;

    ECRYPT_ivsetup(ctx, noncep);
    for (i = 0U; i < len; ++i) {
        outp[i] = 0U;
    }
    ECRYPT_encrypt_bytes(ctx, (u8 *) outp, (u8 *) outp, len);

    return 0;
}

int
crypto_stream_xor_afternm(unsigned char *outp, const unsigned char *inp,
                          unsigned long long len, const unsigned char *noncep,
                          const unsigned char *c)
{
    ECRYPT_ctx * const ctx = (ECRYPT_ctx *) c;

    ECRYPT_ivsetup(ctx, noncep);
    ECRYPT_encrypt_bytes(ctx, (const u8 *) inp, (u8 *) outp, len);

    return 0;
}

int
crypto_stream(unsigned char *out, unsigned long long outlen,
              const unsigned char *n, const unsigned char *k)
{
    unsigned char d[crypto_stream_BEFORENMBYTES];
    crypto_stream_beforenm(d, k);
    crypto_stream_afternm(out, outlen, n, d);

    return 0;
}

int crypto_stream_xor(unsigned char *out, const unsigned char *in,
                      unsigned long long inlen, const unsigned char *n,
                      const unsigned char *k)
{
    unsigned char d[crypto_stream_BEFORENMBYTES];

    crypto_stream_beforenm(d, k);
    crypto_stream_xor_afternm(out, in, inlen, n, d);

    return 0;
}
