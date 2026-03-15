
#define TEST_NAME "codecs2"
#include "cmptest.h"

int
main(void)
{
    unsigned char ip_bytes[16];

    assert(sodium_ip2bin(ip_bytes, "fe80::1%eth 0",
                         strlen("fe80::1%eth 0")) == -1);
    assert(sodium_ip2bin(ip_bytes, "fe80::1%\t",
                         strlen("fe80::1%\t")) == -1);
    assert(sodium_ip2bin(ip_bytes, "fe80::1%eth\n0",
                         strlen("fe80::1%eth\n0")) == -1);
    printf("ip2bin: zone with whitespace rejected: OK\n");

    assert(sodium_ip2bin(ip_bytes, "192.168.1.1%zone",
                         strlen("192.168.1.1%zone")) == -1);
    assert(sodium_ip2bin(ip_bytes, "10.0.0.1%1",
                         strlen("10.0.0.1%1")) == -1);
    printf("ip2bin: IPv4 with zone rejected: OK\n");

    assert(sodium_ip2bin(ip_bytes, "fe80::1%eth0",
                         strlen("fe80::1%eth0")) == 0);
    printf("ip2bin: valid IPv6 zone: OK\n");

    return 0;
}
