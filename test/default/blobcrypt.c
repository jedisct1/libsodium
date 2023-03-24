
#define TEST_NAME "blobcrypt"
#include "cmptest.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <sodium/crypto_generichash.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/utils.h>
#include <string.h>
#include <sys/types.h>

// This testcase has been extracted from https://github.com/jedisct1/blobcrypt
// which is under the following licesning requirements:
//  * Permission to use, copy, modify, and/or distribute this software for any
//  * purpose with or without fee is hereby granted, provided that the above
//  * copyright notice and this permission notice appear in all copies.

int Test_get_key_from_password() {
  // begin function parameters
  int confirm = 1;
  size_t k_bytes = 32;
  unsigned char *k = sodium_malloc(32);
  // end function parameters
  char email[1024U] = "test@email.com";
  char pwd2[1024U] = "password";
  char pwd[1024U] = "password";
  unsigned char salt[16U];
  size_t email_len;
  size_t i;

  email_len = strlen(email);
  i = 0;

  for (int i = 0; i < email_len; i++) {
    email[i] = (char)tolower((unsigned char)email[i]);
  }

  if (confirm != 0) {
    if (strcmp(pwd, pwd2) != 0) {
      sodium_memzero(pwd, sizeof pwd);
      sodium_memzero(pwd2, sizeof pwd2);
    }
    sodium_memzero(pwd2, sizeof pwd2);
  }
  crypto_generichash(salt, sizeof salt, (const unsigned char *)email, email_len,
                     NULL, 0);
  sodium_memzero(email, sizeof email);
  int result =
      crypto_pwhash(k, k_bytes, pwd, strlen(pwd), salt, 3U, 268435456U, 2);
  sodium_memzero(pwd, sizeof pwd);
  sodium_memzero(salt, sizeof salt);
  sodium_free(k);
  return 0;
}

int main() {
  int response = Test_get_key_from_password();
  if (response == 0) {
    printf("OK\n");
    return response;
  } else {
    return -1;
  }
}