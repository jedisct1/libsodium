#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

int schnorr_sign_fixed(const unsigned char *msg, size_t msg_len,
                        const unsigned char *priv_key,
                        unsigned char *sig, size_t *sig_len) {
    printf("  [DEBUG] schnorr_sign_fixed enter\n"); fflush(stdout);
    
    if (!msg || !priv_key || !sig || !sig_len) {
        printf("  [DEBUG] NULL arg\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Getting group...\n"); fflush(stdout);
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!group) {
        printf("  [DEBUG] group NULL\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Getting order...\n"); fflush(stdout);
    BIGNUM *order = BN_new();
    if (!EC_GROUP_get_order(group, order, NULL)) {
        printf("  [DEBUG] get_order failed\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Loading private key...\n"); fflush(stdout);
    BIGNUM *priv = BN_new();
    BN_bin2bn(priv_key, 32, priv);
    
    printf("  [DEBUG] Computing Y = priv*G...\n"); fflush(stdout);
    EC_POINT *Y = EC_POINT_new(group);
    if (!EC_POINT_mul(group, Y, priv, NULL, NULL, NULL)) {
        printf("  [DEBUG] EC_POINT_mul Y failed\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Generating random k...\n"); fflush(stdout);
    BIGNUM *k = BN_new();
    BN_rand(k, 256, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
    BN_mod(k, k, order, NULL);
    
    printf("  [DEBUG] Computing R = k*G...\n"); fflush(stdout);
    EC_POINT *R = EC_POINT_new(group);
    if (!EC_POINT_mul(group, R, k, NULL, NULL, NULL)) {
        printf("  [DEBUG] EC_POINT_mul R failed\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Serializing R...\n"); fflush(stdout);
    unsigned char R_bytes[33];
    if (!EC_POINT_point2oct(group, R, POINT_CONVERSION_COMPRESSED, R_bytes, 33, NULL)) {
        printf("  [DEBUG] point2oct R failed\n"); fflush(stdout);
        return -1;
    }
    
    printf("  [DEBUG] Serializing Y...\n"); fflush(stdout);
    unsigned char Y_bytes[33];
    EC_POINT_point2oct(group, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, NULL);
    
    printf("  [DEBUG] Computing c = SHA256...\n"); fflush(stdout);
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, R_bytes, 33);
    SHA256_Update(&sha, Y_bytes, 33);
    SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32];
    SHA256_Final(c_hash, &sha);
    
    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_mod(c, c, order, NULL);
    
    printf("  [DEBUG] Computing s = k + c*x...\n"); fflush(stdout);
    BIGNUM *s = BN_new();
    BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, priv, order, NULL);
    BN_mod_add(s, k, cx, order, NULL);
    
    printf("  [DEBUG] Building signature...\n"); fflush(stdout);
    memcpy(sig, R_bytes, 33);
    BN_bn2binpad(s, sig + 33, 32);
    *sig_len = 65;
    
    printf("  [DEBUG] Cleanup...\n"); fflush(stdout);
    BN_free(priv); BN_free(k); BN_free(c); BN_free(s); BN_free(cx); BN_free(order);
    EC_POINT_free(Y); EC_POINT_free(R);
    EC_GROUP_free(group);
    
    printf("  [DEBUG] SUCCESS!\n"); fflush(stdout);
    return 0;
}

int main() {
    unsigned char pk[33], sk[32], sig[65];
    size_t siglen = 65;
    
    // Quick keygen
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY *ec = EC_KEY_new();
    EC_KEY_set_group(ec, g);
    EC_KEY_generate_key(ec);
    const BIGNUM *priv = EC_KEY_get0_private_key(ec);
    const EC_POINT *pub = EC_KEY_get0_public_key(ec);
    BN_bn2binpad(priv, sk, 32);
    EC_POINT_point2oct(g, pub, POINT_CONVERSION_COMPRESSED, pk, 33, NULL);
    EC_KEY_free(ec);
    EC_GROUP_free(g);
    
    printf("Keygen done. Signing...\n");
    int ret = schnorr_sign_fixed((unsigned char*)"test", 4, sk, sig, &siglen);
    printf("Result: %d, siglen: %zu\n", ret, siglen);
    
    return 0;
}
