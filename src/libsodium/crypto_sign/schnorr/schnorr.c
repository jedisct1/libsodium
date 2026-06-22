#ifdef HAVE_OPENSSL

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>

int schnorr_keypair(unsigned char *pk, unsigned char *sk) {
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!group) return -1;
    EC_KEY *ec = EC_KEY_new();
    EC_KEY_set_group(ec, group);
    if (!EC_KEY_generate_key(ec)) { EC_KEY_free(ec); EC_GROUP_free(group); return -1; }
    const BIGNUM *priv = EC_KEY_get0_private_key(ec);
    const EC_POINT *pub = EC_KEY_get0_public_key(ec);
    BN_bn2binpad(priv, sk, 32);
    EC_POINT_point2oct(group, pub, POINT_CONVERSION_COMPRESSED, pk, 33, NULL);
    EC_KEY_free(ec); EC_GROUP_free(group);
    return 0;
}

int schnorr_sign(const unsigned char *msg, size_t msg_len,
                  const unsigned char *priv_key,
                  unsigned char *sig, size_t *sig_len) {
    if (!msg || !priv_key || !sig || !sig_len) return -1;
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!group) return -1;
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new(); EC_GROUP_get_order(group, order, ctx);
    BIGNUM *priv = BN_new(); BN_bin2bn(priv_key, 32, priv);
    EC_POINT *Y = EC_POINT_new(group); EC_POINT_mul(group, Y, priv, NULL, NULL, ctx);
    BIGNUM *k = BN_new(); BN_priv_rand_range(k, order);
    EC_POINT *R = EC_POINT_new(group); EC_POINT_mul(group, R, k, NULL, NULL, ctx);
    unsigned char R_bytes[33], Y_bytes[33];
    EC_POINT_point2oct(group, R, POINT_CONVERSION_COMPRESSED, R_bytes, 33, ctx);
    EC_POINT_point2oct(group, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, ctx);
    SHA256_CTX sha; SHA256_Init(&sha);
    SHA256_Update(&sha, R_bytes, 33); SHA256_Update(&sha, Y_bytes, 33);
    SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32]; SHA256_Final(c_hash, &sha);
    BIGNUM *c = BN_new(); BN_bin2bn(c_hash, 32, c); BN_mod(c, c, order, ctx);
    BIGNUM *s = BN_new(); BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, priv, order, ctx); BN_mod_add(s, k, cx, order, ctx);
    memcpy(sig, R_bytes, 33); BN_bn2binpad(s, sig + 33, 32); *sig_len = 65;
    BN_free(priv); BN_free(k); BN_free(c); BN_free(s); BN_free(cx); BN_free(order);
    EC_POINT_free(Y); EC_POINT_free(R); BN_CTX_free(ctx); EC_GROUP_free(group);
    return 0;
}

int schnorr_verify(const unsigned char *sig, size_t sig_len,
                    const unsigned char *msg, size_t msg_len,
                    const unsigned char *pub_key) {
    if (!sig || !msg || !pub_key || sig_len < 65) return -1;
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!group) return -1;
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new(); EC_GROUP_get_order(group, order, ctx);
    EC_POINT *R = EC_POINT_new(group); EC_POINT_oct2point(group, R, sig, 33, ctx);
    BIGNUM *s = BN_new(); BN_bin2bn(sig + 33, 32, s);
    EC_POINT *Y = EC_POINT_new(group); EC_POINT_oct2point(group, Y, pub_key, 33, ctx);
    SHA256_CTX sha; SHA256_Init(&sha);
    SHA256_Update(&sha, sig, 33); SHA256_Update(&sha, pub_key, 33);
    SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32]; SHA256_Final(c_hash, &sha);
    BIGNUM *c = BN_new(); BN_bin2bn(c_hash, 32, c); BN_mod(c, c, order, ctx);
    EC_POINT *sG = EC_POINT_new(group); EC_POINT_mul(group, sG, s, NULL, NULL, ctx);
    EC_POINT *cY = EC_POINT_new(group); EC_POINT_mul(group, cY, NULL, Y, c, ctx);
    EC_POINT *RcY = EC_POINT_new(group); EC_POINT_add(group, RcY, R, cY, ctx);
    int result = (EC_POINT_cmp(group, sG, RcY, ctx) == 0) ? 0 : -1;
    EC_POINT_free(R); EC_POINT_free(Y); EC_POINT_free(sG); EC_POINT_free(cY); EC_POINT_free(RcY);
    BN_free(s); BN_free(c); BN_free(order); BN_CTX_free(ctx); EC_GROUP_free(group);
    return result;
}

int crypto_sign_schnorr_keypair(unsigned char *pk, unsigned char *sk) {
    if (!pk || !sk) return -1;
    return schnorr_keypair(pk, sk);
}

int crypto_sign_schnorr(unsigned char *sig, unsigned long long *siglen,
                         const unsigned char *msg, unsigned long long msglen,
                         const unsigned char *sk) {
    if (!sig || !siglen || !msg || !sk) return -1;
    size_t len;
    int ret = schnorr_sign(msg, (size_t)msglen, sk, sig, &len);
    *siglen = (unsigned long long)len;
    return ret;
}

int crypto_sign_schnorr_verify(const unsigned char *msg, unsigned long long msglen,
                                const unsigned char *sig, unsigned long long siglen,
                                const unsigned char *pk) {
    if (!msg || !sig || !pk) return -1;
    return schnorr_verify(sig, (size_t)siglen, msg, (size_t)msglen, pk);
}

#endif /* HAVE_OPENSSL */
