// Copyright 2016 Google Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");

#include <string>
extern "C" {
  #include <sodium.h>
}

using std::string;

extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
  if (sodium_init() == -1) {
    return 0;
  }

  size_t ciphertext_len = crypto_secretbox_MACBYTES + size;

  unsigned char key[crypto_secretbox_KEYBYTES];
  unsigned char nonce[crypto_secretbox_NONCEBYTES];
  unsigned char ciphertext[ciphertext_len];

  crypto_secretbox_keygen(key);
  randombytes_buf(nonce, sizeof nonce);
  crypto_secretbox_easy(ciphertext, data, size, nonce, key);

  unsigned char decrypted[size];
  crypto_secretbox_open_easy(decrypted, ciphertext, ciphertext_len, nonce, key);

  return 0;
}
