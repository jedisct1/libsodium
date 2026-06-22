#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

int main() {
    unsigned char sk[32], sig[65];
    size_t siglen = 65;
    
    // Keygen
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY *ec = EC_KEY_new();
    EC_KEY_set_group(ec, g);
    EC_KEY_generate_key(ec);
    const BIGNUM *priv_bn = EC_KEY_get0_private_key(ec);
    BN_bn2binpad(priv_bn, sk, 32);
    EC_KEY_free(ec);
    
    printf("SK: %02x%02x%02x%02x...\n", sk[0], sk[1], sk[2], sk[3]);
    
    // Now sign
    BIGNUM *priv = BN_new();
    BN_bin2bn(sk, 32, priv);
    printf("priv bits: %d\n", BN_num_bits(priv));
    printf("priv is zero: %d\n", BN_is_zero(priv));
    
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, NULL);
    
    // Check priv < order
    printf("priv < order: %d\n", BN_cmp(priv, order));
    
    // Create points
    EC_POINT *Y = EC_POINT_new(g);
    printf("EC_POINT_new Y: OK\n"); fflush(stdout);
    
    // Try mul
    printf("Attempting EC_POINT_mul Y...\n"); fflush(stdout);
    int ret = EC_POINT_mul(g, Y, priv, NULL, NULL, NULL);
    printf("EC_POINT_mul Y result: %d\n", ret); fflush(stdout);
    
    if (ret) {
        printf("SIGN WORKS! The bug is in schnorr_sign function!\n");
        
        // Continue with full sign
        BIGNUM *k = BN_new();
        BN_priv_rand_range(k, order);
        
        EC_POINT *R = EC_POINT_new(g);
        EC_POINT_mul(g, R, k, NULL, NULL, NULL);
        
        unsigned char R_bytes[33], Y_bytes[33];
        EC_POINT_point2oct(g, R, POINT_CONVERSION_COMPRESSED, R_bytes, 33, NULL);
        EC_POINT_point2oct(g, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, NULL);
        
        printf("R: %02x%02x...\n", R_bytes[0], R_bytes[1]);
        printf("Full sign: SUCCESS!\n");
    }
    
    BN_free(priv); BN_free(order);
    EC_POINT_free(Y);
    EC_GROUP_free(g);
    
    return 0;
}
