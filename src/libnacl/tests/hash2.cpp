#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::hex;
#include "crypto_hash.h"

int main()
{
  string x = "testing\n";
  string h = crypto_hash(x);
  for (int i = 0;i < h.size();++i) {
    cout << hex << (15 & (int) (h[i] >> 4));
    cout << hex << (15 & (int) h[i]);
  }
  cout << "\n";
  return 0;
}
