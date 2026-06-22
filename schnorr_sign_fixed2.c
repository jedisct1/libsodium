#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

int main() {
    printf("Testing BN_rand...\n");
    BIGNUM *k = BN_new();
    
    printf("BN_new: OK\n");
    fflush(stdout);
    
    // Try BN_rand
    int ret = BN_rand(k, 256, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
    printf("BN_rand result: %d\n", ret);
    fflush(stdout);
    
    // Try BN_priv_rand
    BIGNUM *k2 = BN_new();
    ret = BN_priv_rand(k2, 256, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
    printf("BN_priv_rand result: %d\n", ret);
    fflush(stdout);
    
    // Try BN_rand_range
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, NULL);
    
    BIGNUM *k3 = BN_new();
    ret = BN_rand_range(k3, order);
    printf("BN_rand_range result: %d\n", ret);
    fflush(stdout);
    
    BN_free(k); BN_free(k2); BN_free(k3); BN_free(order);
    EC_GROUP_free(g);
    
    printf("All BN tests done!\n");
    return 0;
}
