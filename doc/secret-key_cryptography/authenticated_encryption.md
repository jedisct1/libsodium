# Secret-key authenticated encryption

## Example

```c
#define MESSAGE ((const unsigned char *) "test")
#define MESSAGE_LEN 4
#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + MESSAGE_LEN)

unsigned char nonce[crypto_secretbox_NONCEBYTES];
unsigned char key[crypto_secretbox_KEYBYTES];
unsigned char ciphertext[CIPHERTEXT_LEN];

randombytes_buf(nonce, sizeof nonce);
randombytes_buf(key, sizeof key);
crypto_secretbox_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, key);

unsigned char decrypted[MESSAGE_LEN];
if (crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key) != 0) {
    /* message forged! */
}
```

## Purpose

This operation:
- Encrypts a message with a key and a nonce to keep it confidential
- Computes an authentication tag. This tag is used to make sure that the message hasn't been tampered with before decrypting it.

A single key is used both to encrypt/sign and verify/decrypt messages. For this reason, it is critical to keep the key confidential.

The nonce doesn't have to be confidential, but it should never ever be reused with the same key. The easiest way to generate a nonce is to use `randombytes_buf()`.

## Combined mode

In combined mode, the authentication tag and the encrypted message are stored together. This is usually what you want, as

```c
int crypto_secretbox_easy(unsigned char *c, const unsigned char *m,
                          unsigned long long mlen, const unsigned char *n,
                          const unsigned char *k);
```

The `crypto_secretbox_easy()` function encrypts a message `m` whose length is `mlen` bytes, with a key `k` and a nonce `n`.

`k` should be `crypto_secretbox_KEYBYTES` bytes and `n` should be `crypto_secretbox_NONCEBYTES` bytes.

`c` should be at least `crypto_secretbox_MACBYTES + mlen` bytes long.

This function writes the authentication tag, whose length is `crypto_secretbox_MACBYTES` bytes, in `c`, immediately followed by the encrypted message, whose length is the same as the plaintext: `mlen`.

`c` and `m` can overlap, making in-place encryption possible. However do not forget that `crypto_secretbox_MACBYTES` extra bytes are required to prepend the tag.

```c
int crypto_secretbox_open_easy(unsigned char *m, const unsigned char *c,
                               unsigned long long clen, const unsigned char *n,
                               const unsigned char *k);
```

The `crypto_secretbox_open_easy()` function verifies and decrypts a ciphertext produced by `crypto_secretbox_easy()`.

`c` is a pointer to an authentication tag + encrypted message combination, as produced by `crypto_secretbox_easy()`.
`clen` is the length of this authentication tag + encrypted message combination. Put differently, `clen` is the number of bytes written by `crypto_secretbox_easy()`, which is `crypto_secretbox_MACBYTES` + the length of the message.

The nonce `n` and the key `k` have to match the used to encrypt and authenticate the message.

The function returns `-1` if the verification fails, and `0` on success.
On success, the decrypted message is stored into `m`.

`m` and `c` can overlap, making in-place decryption possible.

## Detached mode

Some applications may need to store the authentication tag and the encrypted message at different locations.

For this specific use case, "detached" variants of the functions above are available.

```c
int crypto_secretbox_detached(unsigned char *c, unsigned char *mac,
                              const unsigned char *m,
                              unsigned long long mlen,
                              const unsigned char *n,
                              const unsigned char *k);
```

This function encrypts a message `m` of length `mlen` with a key `k` and a nonce `n`, and puts the encrypted message into `c`.
Exactly `mlen` bytes will be put into `c`, since this function does not prepend the authentication tag.
The tag, whose size is `crypto_secretbox_MACBYTES` bytes, will be put into `mac`.

```c
int crypto_secretbox_open_detached(unsigned char *m,
                                   const unsigned char *c,
                                   const unsigned char *mac,
                                   unsigned long long clen,
                                   const unsigned char *n,
                                   const unsigned char *k);
```

The `crypto_secretbox_open_detached()` function verifies and decrypts an encrypted message `c` whose length is `clen`. `clen` doesn't include the tag, so this length is the same as the plaintext.

The plaintext is put into `m` after verifying that `mac` is a valid authentication tag for this ciphertext, with the given nonce `n` and key `k`.

The function returns `-1` if the verification fails, or `0` on success.

## Constants

- `crypto_secretbox_KEYBYTES`
- `crypto_secretbox_MACBYTES`
- `crypto_secretbox_NONCEBYTES`

## Algorithm details

- Encryption: XSalsa20 stream cipher
- Authentication: Poly1305 MAC

## Notes

These functions are specific to libsodium.
The original NaCl `crypto_secretbox` API is also fully supported. However, the `_easy` and `_detached` APIs improve usability, and do not add any overhead.
