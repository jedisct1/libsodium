// Use this runner to execute a input of a fuzz

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

// Forward declare the "fuzz target" interface.
// We deliberately keep this inteface simple and header-free.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    std::ifstream in(argv[i]);
    in.seekg(0, in.end);
    size_t length = in.tellg();
    in.seekg (0, in.beg);
    std::cout << "Reading " << length << " bytes from " << argv[i] << std::endl;
    // Allcate exactly length bytes so that we reliably catch buffer overflows.
    std::vector<char> bytes(length);
    in.read(bytes.data(), bytes.size());
    assert(in);
    LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
                           bytes.size());
    std::cout << "Execution successful" << std::endl;
  }
}
