#include <string>
extern "C" {
  #include <sodium.h>
}

using std::string;

extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
  if (sodium_init() == -1) {
    return 0;
  }

  unsigned char key[crypto_auth_KEYBYTES];
  unsigned char mac[crypto_auth_BYTES];

  crypto_auth_keygen(key);

  crypto_auth(mac, data, size, key);
  crypto_auth_verify(mac, data, size, key);

  return 0;
}
