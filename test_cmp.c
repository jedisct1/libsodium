#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <stdio.h>

int main() {
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    
    // Test: G == G?
    EC_POINT *G1 = EC_POINT_new(g);
    EC_POINT *G2 = EC_POINT_new(g);
    BIGNUM *one = BN_new(); BN_one(one);
    
    EC_POINT_mul(g, G1, one, NULL, NULL, ctx);
    EC_POINT_mul(g, G2, one, NULL, NULL, ctx);
    
    int cmp = EC_POINT_cmp(g, G1, G2, ctx);
    printf("G == G: %d (0=equal)\n", cmp);
    
    // Test: G == 2*G?
    BIGNUM *two = BN_new();
    BN_set_word(two, 2);
    EC_POINT *G2_mul = EC_POINT_new(g);
    EC_POINT_mul(g, G2_mul, two, NULL, NULL, ctx);
    
    int cmp2 = EC_POINT_cmp(g, G1, G2_mul, ctx);
    printf("G == 2*G: %d (non-zero=not equal)\n", cmp2);
    
    EC_POINT_free(G1); EC_POINT_free(G2); EC_POINT_free(G2_mul);
    BN_free(one); BN_free(two);
    BN_CTX_free(ctx);
    EC_GROUP_free(g);
    
    return 0;
}
