#ifndef FAKE_RANDOM_H_
#define FAKE_RANDOM_H_

#include <sodium.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>

static const unsigned char * SEED_DATA;
static size_t SEED_SIZE;

static const char *
fake_implementation_name(void) {
  return "fake_random";
}

static void
fake_random_buffer(void * const buf, const size_t size) {
  static unsigned char seed[randombytes_SEEDBYTES];
  memset(seed, '0', randombytes_SEEDBYTES);

  size_t boundary = std::min((size_t) randombytes_SEEDBYTES, SEED_SIZE);
  memcpy(&seed, SEED_DATA, boundary);

  randombytes_buf_deterministic(buf, size, seed);
}

struct randombytes_implementation fake_random = {
  .implementation_name = fake_implementation_name,
  .random = NULL,
  .stir = NULL,
  .uniform = NULL,
  .buf = fake_random_buffer,
  .close = NULL
};

void
setup_fake_random(const unsigned char * seed, const size_t seed_size) {
  SEED_DATA = seed;
  SEED_SIZE = seed_size;

  int fake_random_set = randombytes_set_implementation(&fake_random);
  assert(fake_random_set == 0);

  assert(randombytes_implementation_name() == "fake_random");
  int initialized = sodium_init();
  assert(initialized >= 0);
}

#endif // FAKE_RANDOM_H_
