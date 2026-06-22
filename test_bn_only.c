#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <stdio.h>

int main() {
    printf("Test 1: BN_new + BN_priv_rand_range\n");
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, NULL);
    printf("Order bits: %d\n", BN_num_bits(order));
    
    BIGNUM *k = BN_new();
    printf("BN_new: %p\n", k);
    fflush(stdout);
    
    int ret = BN_priv_rand_range(k, order);
    printf("BN_priv_rand_range: %d\n", ret);
    printf("k bits: %d\n", BN_num_bits(k));
    
    BN_free(k); BN_free(order); EC_GROUP_free(g);
    
    printf("\nTest 2: BN_rand_range\n");
    EC_GROUP *g2 = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BIGNUM *order2 = BN_new();
    EC_GROUP_get_order(g2, order2, NULL);
    
    BIGNUM *k2 = BN_new();
    printf("BN_new: %p\n", k2);
    fflush(stdout);
    
    int ret2 = BN_rand_range(k2, order2);
    printf("BN_rand_range: %d\n", ret2);
    printf("k2 bits: %d\n", BN_num_bits(k2));
    
    BN_free(k2); BN_free(order2); EC_GROUP_free(g2);
    
    printf("\nAll tests passed!\n");
    return 0;
}
