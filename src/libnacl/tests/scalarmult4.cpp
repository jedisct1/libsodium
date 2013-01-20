#include <iostream>
#include <iomanip>
#include <string>
using std::string;
using std::cout;
using std::setfill;
using std::setw;
using std::hex;
#include "crypto_scalarmult_curve25519.h"

char bobsk_bytes[32] = {
 0x5d,0xab,0x08,0x7e,0x62,0x4a,0x8a,0x4b
,0x79,0xe1,0x7f,0x8b,0x83,0x80,0x0e,0xe6
,0x6f,0x3b,0xb1,0x29,0x26,0x18,0xb6,0xfd
,0x1c,0x2f,0x8b,0x27,0xff,0x88,0xe0,0xeb
} ;

main()
{
  int i;
  cout << setfill('0');
  string bobsk(bobsk_bytes,sizeof bobsk_bytes);
  string bobpk = crypto_scalarmult_curve25519_base(bobsk);
  for (i = 0;i < bobpk.size();++i) {
    unsigned char c = bobpk[i];
    if (i > 0) cout << ","; else cout << " ";
    cout << "0x" << hex << setw(2) << (unsigned int) c;
    if (i % 8 == 7) cout << "\n";
  }
  return 0;
}
