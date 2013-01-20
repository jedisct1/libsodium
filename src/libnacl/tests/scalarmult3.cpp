#include <iostream>
#include <iomanip>
#include <string>
using std::string;
using std::cout;
using std::setfill;
using std::setw;
using std::hex;
#include "crypto_scalarmult_curve25519.h"

char alicesk_bytes[32] = {
 0x77,0x07,0x6d,0x0a,0x73,0x18,0xa5,0x7d
,0x3c,0x16,0xc1,0x72,0x51,0xb2,0x66,0x45
,0xdf,0x4c,0x2f,0x87,0xeb,0xc0,0x99,0x2a
,0xb1,0x77,0xfb,0xa5,0x1d,0xb9,0x2c,0x2a
} ;

main()
{
  int i;
  cout << setfill('0');
  string alicesk(alicesk_bytes,sizeof alicesk_bytes);
  string alicepk = crypto_scalarmult_curve25519_base(alicesk);
  for (i = 0;i < alicepk.size();++i) {
    unsigned char c = alicepk[i];
    if (i > 0) cout << ","; else cout << " ";
    cout << "0x" << hex << setw(2) << (unsigned int) c;
    if (i % 8 == 7) cout << "\n";
  }
  return 0;
}
