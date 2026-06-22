#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <stdio.h>

int main() {
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    
    // Test compressed point 02 (Y even)
    unsigned char point_02[33] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    EC_POINT *P = EC_POINT_new(group);
    int ret = EC_POINT_oct2point(group, P, point_02, 33, NULL);
    printf("Parse 02 point: %d (1=success)\n", ret);
    
    // Test compressed point from actual sig
    unsigned char sig_R[33] = {0x03, 0x1a, 0xc2, 0x4c, 0x2a, 0x3d, 0x0d, 0x07};
    EC_POINT *R = EC_POINT_new(group);
    int ret2 = EC_POINT_oct2point(group, R, sig_R, 33, NULL);
    printf("Parse sig R: %d (1=success)\n", ret2);
    
    // Test: create point from generator, then re-parse
    EC_POINT *G = EC_POINT_new(group);
    EC_POINT_mul(group, G, BN_new(), NULL, NULL, NULL);  // G = 1*G (dummy)
    unsigned char G_bytes[33];
    EC_POINT_point2oct(group, G, POINT_CONVERSION_COMPRESSED, G_bytes, 33, NULL);
    printf("G serialized: %02x%02x%02x...\n", G_bytes[0], G_bytes[1], G_bytes[2]);
    
    EC_POINT *G2 = EC_POINT_new(group);
    int ret3 = EC_POINT_oct2point(group, G2, G_bytes, 33, NULL);
    printf("Re-parse G: %d (1=success)\n", ret3);
    
    EC_POINT_free(P); EC_POINT_free(R); EC_POINT_free(G); EC_POINT_free(G2);
    EC_GROUP_free(group);
    return 0;
}
