#include <string>
extern "C" {
  #include <sodium.h>
}

using std::string;

extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
  if (sodium_init() == -1) {
    return 0;
  }

  unsigned char hash[crypto_generichash_BYTES];
  crypto_generichash(hash, sizeof hash,
                     data, size,
                     NULL, 0);
  return 0;
}
