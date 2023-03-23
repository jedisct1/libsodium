
#define TEST_NAME "dovecot-1"
#include "cmptest.h"

// This testcase has been extracted from https://github.com/dovecot/core which
// is under the LGPL license.

struct password_generate_params {
  const char *user;
  unsigned int rounds;
};

int Test_generate_argon2id(const char *plaintext,
                           const struct password_generate_params *params) {
  unsigned long long rounds = params->rounds;
  size_t memlimit;
  char result[crypto_pwhash_argon2id_STRBYTES];
  memset(&result, 0, sizeof(*(&result)));

  if (rounds == 0) {
    rounds = crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE;
  }

  if (rounds >= crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE)
    memlimit = crypto_pwhash_argon2id_MEMLIMIT_SENSITIVE;
  else if (rounds >= crypto_pwhash_argon2id_OPSLIMIT_MODERATE)
    memlimit = crypto_pwhash_argon2id_MEMLIMIT_MODERATE;
  else
    memlimit = crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE;

  if (rounds < 3) {
    rounds = 3;
  }
  if (crypto_pwhash_argon2id_str(result, plaintext, strlen(plaintext), rounds,
                                 memlimit) < 0)
    return -1;
  return 0;
}

int main() {

  struct password_generate_params *p =
      (struct password_generate_params *)malloc(
          sizeof(struct password_generate_params));
  p->rounds = 0;
  p->user = "testuser1";
  int response = Test_generate_argon2id("test", p);
  if (response == 0) {
    printf("OK\n");
    return response;
  } else {
    return -1;
  }
}
