#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

int schnorr_sign_final(const unsigned char *msg, size_t msg_len,
                        const unsigned char *priv_key,
                        unsigned char *sig, size_t *sig_len) {
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!group) return -1;
    
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(group, order, NULL);
    
    BIGNUM *priv = BN_new();
    BN_bin2bn(priv_key, 32, priv);
    
    // Create and init points with group
    EC_POINT *Y = EC_POINT_new(group);
    EC_POINT *R = EC_POINT_new(group);
    
    // Generate random k safely
    BIGNUM *k = BN_new();
    BN_priv_rand_range(k, order);  // Use priv_rand_range (crypto-safe)
    
    // Compute points
    EC_POINT_mul(group, Y, priv, NULL, NULL, NULL);
    EC_POINT_mul(group, R, k, NULL, NULL, NULL);
    
    // Serialize
    unsigned char R_bytes[33], Y_bytes[33];
    EC_POINT_point2oct(group, R, POINT_CONVERSION_COMPRESSED, R_bytes, 33, NULL);
    EC_POINT_point2oct(group, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, NULL);
    
    // Hash
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
    
    // s = k + c*x
    BIGNUM *s = BN_new();
    BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, priv, order, NULL);
    BN_mod_add(s, k, cx, order, NULL);
    
    // Build signature
    memcpy(sig, R_bytes, 33);
    BN_bn2binpad(s, sig + 33, 32);
    *sig_len = 65;
    
    BN_free(priv); BN_free(k); BN_free(c); BN_free(s); BN_free(cx); BN_free(order);
    EC_POINT_free(Y); EC_POINT_free(R);
    EC_GROUP_free(group);
    
    return 0;
}

int main() {
    unsigned char pk[33], sk[32], sig[65];
    size_t siglen = 65;
    
    // Keygen
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY *ec = EC_KEY_new();
    EC_KEY_set_group(ec, g);
    EC_KEY_generate_key(ec);
    const BIGNUM *priv = EC_KEY_get0_private_key(ec);
    const EC_POINT *pub = EC_KEY_get0_public_key(ec);
    BN_bn2binpad(priv, sk, 32);
    EC_POINT_point2oct(g, pub, POINT_CONVERSION_COMPRESSED, pk, 33, NULL);
    EC_KEY_free(ec);
    
    printf("Keygen OK. Signing...\n");
    int ret = schnorr_sign_final((unsigned char*)"test", 4, sk, sig, &siglen);
    printf("Sign result: %d, siglen: %zu\n", ret, siglen);
    printf("Sig[0..7]: %02x%02x%02x%02x%02x%02x%02x%02x\n",
           sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7]);
    
    return 0;
}
