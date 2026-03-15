#define TEST_NAME "codecs3"
#include "cmptest.h"

int
main(void)
{
    unsigned char ip_bytes[16];

    assert(sodium_ip2bin(ip_bytes, "::1:2:3:4:5:6:7:8",
                         strlen("::1:2:3:4:5:6:7:8")) == -1);
    printf("ipv6 double-colon full address: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7",
                         strlen("1:2:3:4:5:6:7")) == -1);
    printf("ipv6 too few groups: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:8:9",
                         strlen("1:2:3:4:5:6:7:8:9")) == -1);
    printf("ipv6 too many groups: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:8:a",
                         strlen("1:2:3:4:5:6:7:8:a")) == -1);
    printf("ipv6 too many groups at end: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:192.168.1.1",
                         strlen("1:2:3:4:5:6:7:192.168.1.1")) == -1);
    printf("ipv6 embedded IPv4 too many groups: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "::999.1.1.1",
                         strlen("::999.1.1.1")) == -1);
    printf("ipv6 embedded IPv4 invalid: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "::192.168.1.1",
                         strlen("::192.168.1.1")) == 0);
    printf("ipv6 embedded IPv4 valid: ok\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:8:",
                         strlen("1:2:3:4:5:6:7:8:")) == -1);
    printf("ipv6 trailing colon: rejected\n");

    assert(sodium_ip2bin(ip_bytes, ":1:2:3:4:5:6:7:8",
                         strlen(":1:2:3:4:5:6:7:8")) == -1);
    printf("ipv6 single leading colon: rejected\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:7:8",
                         strlen("1:2:3:4:5:6:7:8")) == 0);
    assert(ip_bytes[0] == 0 && ip_bytes[1] == 1);
    assert(ip_bytes[14] == 0 && ip_bytes[15] == 8);
    printf("ipv6 full 8 groups: ok\n");

    assert(sodium_ip2bin(ip_bytes, "1:2:3:4:5:6:1.2.3.4",
                         strlen("1:2:3:4:5:6:1.2.3.4")) == 0);
    printf("ipv6 embedded IPv4 6 groups: ok\n");

    return 0;
}
