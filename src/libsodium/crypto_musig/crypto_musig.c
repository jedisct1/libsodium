#include <assert.h>
#include <string.h>

#include "crypto_musig.h"
#include "crypto_generichash_blake2b.h"
#include "crypto_scalarmult_ristretto255.h"
#include "utils.h"
#include "core.h"

// These values are used to seed three separate and
// independent hash domains as required by the MuSig
// algorithm.

// crypto_hash_sha256(hash_agg_key, "agg", 3);
static const unsigned char hash_agg_key[] = {
    0x54, 0x8e, 0x75, 0x0b, 0x9c, 0xad, 0xc2, 0xa9,
    0x33, 0x6f, 0xcb, 0x9b, 0xcb, 0x33, 0x62, 0xd1,
    0xb9, 0x51, 0xa8, 0xd7, 0x31, 0x00, 0x81, 0xaa,
    0x20, 0x0d, 0x27, 0x7e, 0x19, 0x6c, 0x4f, 0x0b
};

// crypto_hash_sha256(hash_com_key, "com", 3);
static const unsigned char hash_com_key[] = {
    0x71, 0xb4, 0xf3, 0xa3, 0x74, 0x8c, 0xd6, 0x84,
    0x3c, 0x01, 0xe2, 0x93, 0xe7, 0x01, 0xfc, 0xe7,
    0x69, 0xf5, 0x23, 0x81, 0x82, 0x1e, 0x21, 0xda,
    0xf2, 0xff, 0x4f, 0xe9, 0xea, 0x57, 0xa6, 0xf3
};

// crypto_hash_sha256(hash_sig_key, "sig", 3);
static const unsigned char hash_sig_key[] = {
    0xa5, 0x43, 0x99, 0x7d, 0x84, 0xf1, 0x27, 0x98,
    0x35, 0x0c, 0x09, 0xbd, 0xef, 0x2c, 0xdb, 0x17,
    0x1b, 0xf4, 0x1e, 0xd3, 0xe4, 0xa5, 0xf8, 0x08,
    0xaf, 0x2f, 0xeb, 0x0c, 0x56, 0x26, 0x30, 0x09
};

size_t
crypto_musig_statebytes(void)
{
    return sizeof(crypto_musig_state);
}

size_t
crypto_musig_bytes(void)
{
    return crypto_musig_BYTES;
}

size_t
crypto_musig_seedbytes(void)
{
    return crypto_musig_SEEDBYTES;
}

size_t
crypto_musig_publickeybytes(void)
{
    return crypto_musig_PUBLICKEYBYTES;
}

size_t
crypto_musig_secretkeybytes(void)
{
    return crypto_musig_SECRETKEYBYTES;
}

size_t
crypto_musig_rbytes(void)
{
    return crypto_musig_RBYTES;
}

size_t
crypto_musig_kstbytes(void)
{
    return crypto_musig_KSTBYTES;
}

size_t
crypto_musig_messagebytes_max(void)
{
    return crypto_musig_MESSAGEBYTES_MAX;
}

const char *
crypto_musig_primitive(void)
{
    return crypto_musig_PRIMITIVE;
}

int
crypto_musig_seed_keypair(unsigned char *pk, unsigned char *sk,
                          const unsigned char *seed)
{
    unsigned char h[crypto_core_ristretto255_NONREDUCEDSCALARBYTES];
    assert(0 == crypto_generichash_blake2b(h, sizeof h,
                seed, crypto_musig_SEEDBYTES, NULL, 0));
    crypto_core_ristretto255_scalar_reduce(sk, h);
    return crypto_scalarmult_ristretto255_base(pk, sk);
}

int
crypto_musig_keypair(unsigned char *pk, unsigned char *sk)
{
    crypto_core_ristretto255_scalar_random(sk);
    return crypto_scalarmult_ristretto255_base(pk, sk);
}

static void
_crypto_musig_a(unsigned char *a, const unsigned char *hl,
                const unsigned char *pk)
{
    crypto_generichash_blake2b_state hst;
    unsigned char h[crypto_core_ristretto255_NONREDUCEDSCALARBYTES];

    assert(0 == crypto_generichash_blake2b_init(&hst, hash_agg_key,
                sizeof hash_agg_key, sizeof h));
    assert(0 == crypto_generichash_blake2b_update(&hst, hl,
                crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, pk,
                crypto_musig_PUBLICKEYBYTES));
    assert(0 == crypto_generichash_blake2b_final(&hst, h, sizeof h));
    crypto_core_ristretto255_scalar_reduce(a, h);
}

static int
_crypto_musig_publickey(unsigned char *pk, unsigned char *a,
                        const crypto_musig_peer *peers)
{
    crypto_generichash_blake2b_state hst;
    unsigned char hl[crypto_core_ristretto255_NONREDUCEDSCALARBYTES];
    unsigned char a_tmp[crypto_musig_KSTBYTES];
    const crypto_musig_peer *p;

    if (!peers || !crypto_core_ristretto255_is_valid_point(peers->pk)) {
        sodium_misuse();
    }

    // The MuSig algorithm requires repeatedly hashing L (the set of public
    // keys involved in the signature) and each public key to compute the
    // 'a' coefficients.
    //
    // As described in section 4.3 of https://eprint.iacr.org/2018/068.pdf
    // it is possible to replace L with a cryptographic hash thereof ('hl'
    // in the code below) without affecting the security of the algorithm.
    assert(0 == crypto_generichash_blake2b_init(&hst, hash_agg_key,
                sizeof hash_agg_key,
                crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    for (p=peers; p; p=p->next) {
        if (!crypto_core_ristretto255_is_valid_point(p->pk)) {
            return -1;
        }
        assert(0 == crypto_generichash_blake2b_update(&hst, p->pk,
                    crypto_musig_PUBLICKEYBYTES));
    }
    assert(0 == crypto_generichash_blake2b_final(&hst, hl, sizeof hl));

    _crypto_musig_a(a_tmp, hl, peers->pk);
    if (a && peers->is_self) {
        memcpy(a, a_tmp, crypto_musig_KSTBYTES);
    }
    if (crypto_scalarmult_ristretto255(pk, a_tmp, peers->pk)) {
        return -1;
    }

    for (p=peers->next; p; p=p->next) {
        unsigned char tmp[crypto_musig_PUBLICKEYBYTES];
        _crypto_musig_a(a_tmp, hl, p->pk);
        if (a && p->is_self) {
            memcpy(a, a_tmp, crypto_musig_KSTBYTES);
        }
        if (crypto_scalarmult_ristretto255(tmp, a_tmp, p->pk) ||
                crypto_core_ristretto255_add(pk, pk, tmp)) {
            return -1;
        }
    }
    return 0;
}

void
crypto_musig_init(crypto_musig_state *st)
{
    sodium_memzero(st, sizeof(crypto_musig_state));
}

int
crypto_musig_add(crypto_musig_state *st, const unsigned char *pk)
{
    crypto_musig_peer **p = &st->peers;
    crypto_musig_peer *np = NULL;

    if (!crypto_core_ristretto255_is_valid_point(pk)) {
        sodium_misuse();
    }

    while (*p) {
        int c = memcmp(pk, (*p)->pk, crypto_musig_PUBLICKEYBYTES);
        if (c == 0) {
            // prevent keys from being added more than once
            sodium_misuse();
        } else if (c < 0) {
            // keep the list of keys sorted, so that the signature
            // is independent of the order in which keys are added
            break;
        }
        p = &(*p)->next;
    }

    np = sodium_malloc(sizeof(crypto_musig_peer));
    if (!np) {
        return -1;
    }
    sodium_memzero(np, sizeof(crypto_musig_peer));
    memcpy(np->pk, pk, crypto_musig_PUBLICKEYBYTES);
    np->next = *p;
    *p = np;
    return 0;
}

void
crypto_musig_begin(crypto_musig_state *st, unsigned char *r, unsigned char *t)
{
    crypto_core_ristretto255_scalar_random(st->k);
    assert(0 == crypto_scalarmult_ristretto255_base(r, st->k));
    assert(0 == crypto_generichash_blake2b(t, crypto_musig_KSTBYTES,
                r, crypto_musig_RBYTES, hash_com_key, sizeof hash_com_key));
}

int
crypto_musig_commit(crypto_musig_state *st, const unsigned char *pk,
                    const unsigned char *t)
{
    if (!crypto_core_ristretto255_is_valid_point(pk)) {
        sodium_misuse();
    }
    for (crypto_musig_peer *p = st->peers; p; p = p->next) {
        if (sodium_memcmp(pk, p->pk, crypto_musig_PUBLICKEYBYTES) == 0) {
            memcpy(p->t, t, crypto_musig_KSTBYTES);
            return 0;
        }
    }
    return -1;
}

int
crypto_musig_check(crypto_musig_state *st, const unsigned char *pk,
                   const unsigned char *r)
{
    if (!crypto_core_ristretto255_is_valid_point(pk) ||
            !crypto_core_ristretto255_is_valid_point(r)) {
        sodium_misuse();
    }
    for (crypto_musig_peer *p = st->peers; p; p = p->next) {
        if (sodium_memcmp(pk, p->pk, crypto_musig_PUBLICKEYBYTES) == 0) {
            unsigned char t[crypto_musig_KSTBYTES];
            assert(0 == crypto_generichash_blake2b(t, crypto_musig_KSTBYTES,
                        r, crypto_musig_RBYTES, hash_com_key,
                        sizeof hash_com_key));
            if (sodium_memcmp(t, p->t, crypto_musig_KSTBYTES) == 0) {
                memcpy(p->r, r, crypto_musig_RBYTES);
                return 0;
            }
        }
    }
    return -1;
}

int
crypto_musig_sign(crypto_musig_state *st, unsigned char *s,
                  const unsigned char *sk, const unsigned char *m,
                  unsigned long long mlen)
{
    crypto_generichash_blake2b_state hst;
    unsigned char h[crypto_core_ristretto255_NONREDUCEDSCALARBYTES];
    unsigned char a[crypto_musig_KSTBYTES];
    unsigned char pk[crypto_musig_PUBLICKEYBYTES];
    unsigned char r[crypto_musig_RBYTES];
    crypto_musig_peer *p, *self = NULL;

    if (!st->peers) {
        sodium_misuse();
    }

    if (crypto_scalarmult_ristretto255_base(pk, sk)) {
        return -1;
    }

    for (p=st->peers; p; p=p->next) {
        unsigned char t[crypto_musig_KSTBYTES];
        if (!crypto_core_ristretto255_is_valid_point(p->r) ||
                !crypto_core_ristretto255_is_valid_point(p->pk)) {
            return -1;
        }
        assert(0 == crypto_generichash_blake2b(t, crypto_musig_KSTBYTES,
                    p->r, crypto_musig_RBYTES, hash_com_key,
                    sizeof hash_com_key));
        if (sodium_memcmp(t, p->t, crypto_musig_KSTBYTES)) {
            return -1;
        }
        p->is_self = !sodium_memcmp(pk, p->pk, crypto_musig_PUBLICKEYBYTES);
        if (p->is_self) {
            self = p;
        }
    }

    if (!self) {
        return -1;
    }

    if (crypto_scalarmult_ristretto255_base(r, st->k) ||
            sodium_memcmp(r, self->r, crypto_musig_RBYTES)) {
        // check that k and r correspond
        sodium_misuse();
    }

    memcpy(r, st->peers->r, crypto_musig_RBYTES);
    for (p=st->peers->next; p; p=p->next) {
        if (crypto_core_ristretto255_add(r, r, p->r)) {
            return -1;
        }
    }

    if (_crypto_musig_publickey(pk, a, st->peers)) {
        return -1;
    }

    assert(0 == crypto_generichash_blake2b_init(&hst, hash_sig_key,
                sizeof hash_sig_key,
                crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, pk,
                crypto_musig_PUBLICKEYBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, r,
                crypto_musig_RBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, m, mlen));
    assert(0 == crypto_generichash_blake2b_final(&hst, h,
                crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    crypto_core_ristretto255_scalar_reduce(self->s, h);
    crypto_core_ristretto255_scalar_mul(self->s, self->s, a);
    crypto_core_ristretto255_scalar_mul(self->s, self->s, sk);
    crypto_core_ristretto255_scalar_add(self->s, self->s, st->k);
    memcpy(s, self->s, crypto_musig_KSTBYTES);
    // wipe k to prevent reusing it
    sodium_memzero(st->k, crypto_musig_KSTBYTES);
    return 0;
}

int
crypto_musig_update(crypto_musig_state *st, const unsigned char *pk,
                    const unsigned char *s)
{
    if (!crypto_core_ristretto255_is_valid_point(pk)) {
        return -1;
    }
    for (crypto_musig_peer *p = st->peers; p; p = p->next) {
        if (sodium_memcmp(pk, p->pk, crypto_musig_PUBLICKEYBYTES) == 0) {
            memcpy(p->s, s, crypto_musig_KSTBYTES);
            return 0;
        }
    }
    return -1;
}

int
crypto_musig_key(crypto_musig_state *st, unsigned char *pk)
{
    return _crypto_musig_publickey(pk, NULL, st->peers);
}

int
crypto_musig_final(crypto_musig_state *st, unsigned char *sm,
                   unsigned long long *smlen_p, const unsigned char *m,
                   unsigned long long mlen)
{
    memmove(sm + crypto_musig_BYTES, m, mlen);
    if (crypto_musig_final_detached(st, sm, m, mlen) != 0) {
        if (smlen_p != NULL) {
            *smlen_p = 0;
        }
        sodium_memzero(sm, mlen + crypto_musig_BYTES);
        return -1;
    }

    if (smlen_p != NULL) {
        *smlen_p = mlen + crypto_musig_BYTES;
    }
    return 0;
}

int
crypto_musig_final_detached(crypto_musig_state *st, unsigned char *sig,
                            const unsigned char *m, unsigned long long mlen)
{
    crypto_musig_peer *p;
    unsigned char *r = sig;
    unsigned char *s = sig + crypto_musig_RBYTES;

    if (!st->peers) {
        sodium_misuse();
    }
    memcpy(s, st->peers->s, crypto_musig_KSTBYTES);
    for (p=st->peers->next; p; p=p->next) {
        crypto_core_ristretto255_scalar_add(s, s, p->s);
    }
    memcpy(r, st->peers->r, crypto_musig_RBYTES);
    for (p=st->peers->next; p; p=p->next) {
        if (crypto_core_ristretto255_add(r, r, p->r)) {
            return -1;
        }
    }
    return 0;
}

int
crypto_musig_verify(unsigned char *m, unsigned long long *mlen_p,
                    const unsigned char *sm, unsigned long long smlen,
                    const unsigned char *pk)
{
    unsigned long long mlen;

    if (smlen < crypto_musig_BYTES || smlen - crypto_musig_BYTES >
            crypto_musig_MESSAGEBYTES_MAX) {
        goto badsig;
    }
    mlen = smlen - crypto_musig_BYTES;
    if (crypto_musig_verify_detached(sm, sm + crypto_musig_BYTES,
                mlen, pk) != 0) {
        if (m != NULL) {
            sodium_memzero(m, mlen);
        }
        goto badsig;
    }
    if (mlen_p != NULL) {
        *mlen_p = mlen;
    }
    if (m != NULL) {
        memmove(m, sm + crypto_musig_BYTES, mlen);
    }
    return 0;

badsig:
    if (mlen_p != NULL) {
        *mlen_p = 0;
    }
    return -1;
}

int
crypto_musig_verify_detached(const unsigned char *sig, const unsigned char *m,
                             unsigned long long mlen, const unsigned char *pk)
{
    const unsigned char *r = sig;
    const unsigned char *s = sig + crypto_musig_RBYTES;
    unsigned char gs[crypto_musig_RBYTES];
    unsigned char gv[crypto_musig_RBYTES];
    unsigned char e[crypto_musig_KSTBYTES];
    unsigned char h[crypto_core_ristretto255_NONREDUCEDSCALARBYTES];
    crypto_generichash_blake2b_state hst;

    assert(0 == crypto_generichash_blake2b_init(&hst, hash_sig_key,
                sizeof hash_sig_key,
                crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, pk,
                crypto_musig_PUBLICKEYBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, r,
                crypto_musig_RBYTES));
    assert(0 == crypto_generichash_blake2b_update(&hst, m, mlen));
    assert(0 == crypto_generichash_blake2b_final(&hst, h,
            crypto_core_ristretto255_NONREDUCEDSCALARBYTES));
    crypto_core_ristretto255_scalar_reduce(e, h);
    if (crypto_scalarmult_ristretto255_base(gs, s) ||
            crypto_scalarmult_ristretto255(gv, e, pk) ||
            crypto_core_ristretto255_add(gv, gv, r)) {
        return -1;
    }
    return sodium_memcmp(gs, gv, crypto_musig_RBYTES);
}

void
crypto_musig_cleanup(crypto_musig_state *st)
{
    while (st->peers) {
        crypto_musig_peer *p = st->peers->next;
        sodium_free(st->peers);
        st->peers = p;
    }
    sodium_memzero(st, sizeof(crypto_musig_state));
}

