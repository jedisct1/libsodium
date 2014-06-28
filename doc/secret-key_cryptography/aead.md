# Authenticated Encryption with Additional Data

## Example

```c
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4
#define ADDITIONAL_DATA (const unsigned char *) "123456"
#define ADDITIONAL_DATA_LEN 6

unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
unsigned char key[crypto_aead_chacha20poly1305_KEYBYTES];
unsigned char ciphertext[MESSAGE_LEN + crypto_aead_chacha20poly1305_ABYTES];
unsigned long long ciphertext_len;

randombytes_buf(key, sizeof key);
randombytes_buf(nonce, sizeof nonce);

crypto_aead_chacha20poly1305_encrypt(ciphertext, &ciphertext_len,
                                     MESSAGE, MESSAGE_LEN,
                                     ADDITIONAL_DATA, ADDITIONAL_DATA_LEN,
                                     NULL, nonce, key);

unsigned char decrypted[MESSAGE_LEN];
unsigned long long decrypted_len;
if (crypto_aead_chacha20poly1305_decrypt(decrypted, &decrypted_len,
                                         NULL,
                                         ciphertext, ciphertext_len,
                                         ADDITIONAL_DATA,
                                         ADDITIONAL_DATA_LEN,
                                         nonce, key) != 0) {
    /* message forged! */
}
```

## Purpose

This operation:
- Encrypts a message with a key and a nonce to keep it confidential
- Computes an authentication tag. This tag is used to make sure that the message, as well as optional, non-confidential (non-encrypted) data, haven't been tampered with.

A typical use case for additional data is to store protocol-specific metadata about the message, such as its length, and its encoding.

## Usage

```c
int crypto_aead_chacha20poly1305_encrypt(unsigned char *c,
                                         unsigned long long *clen,
                                         const unsigned char *m,
                                         unsigned long long mlen,
                                         const unsigned char *ad,
                                         unsigned long long adlen,
                                         const unsigned char *nsec,
                                         const unsigned char *npub,
                                         const unsigned char *k);
```

The `crypto_aead_chacha20poly1305_encrypt()` function encrypts a message `m` whose length is `mlen` bytes using a secret key `k` (`crypto_aead_chacha20poly1305_KEYBYTES` bytes) and a public nonce `npub` (`crypto_aead_chacha20poly1305_NPUBBYTES` bytes).

The encrypted message, as well as a tag authenticating both the confidential message `m` and `adlen` bytes of non-confidential data `ad`, are put into `c`.

`ad` can also be a `NULL` pointer if no additional data are required.

At most `mlen + crypto_aead_chacha20poly1305_ABYTES` bytes are put into `c`, and the actual number of bytes is stored into `clen` if `clen` is not a `NULL` pointer.

`nsec` is not used by this particular construction and should always be `NULL`.

The public nonce `npub` should never ever be reused with the same key. The easiest way to generate it is to use `randombytes_buf()`.

```c
int crypto_aead_chacha20poly1305_decrypt(unsigned char *m,
                                         unsigned long long *mlen,
                                         unsigned char *nsec,
                                         const unsigned char *c,
                                         unsigned long long clen,
                                         const unsigned char *ad,
                                         unsigned long long adlen,
                                         const unsigned char *npub,
                                         const unsigned char *k);
```

The `crypto_aead_chacha20poly1305_decrypt()` function verifies that the ciphertext `c` (as produced by `crypto_aead_chacha20poly1305_encrypt()`) includes a valid tag using a secret key `k`, a public nonce `npub`, and additional data `ad` (`adlen` bytes).

`ad` can be a `NULL` pointer if no additional data are required.

`nsec` is not used by this particular construction and should always be `NULL`.

The function returns `-1` is the verification fails.

If the verification succeeds, the function returns `-0`, puts the decrypted message into `m` and stores its actual number of bytes into `mlen` if `mlen` is not a `NULL` pointer.

At most `clen - crypto_aead_chacha20poly1305_ABYTES` bytes will be put into `m`.

## Constants

- `crypto_aead_chacha20poly1305_KEYBYTES`
- `crypto_aead_chacha20poly1305_NPUBBYTES`
- `crypto_aead_chacha20poly1305_ABYTES`

## Algorithm details

- Encryption: ChaCha20 stream cipher
- Authentication: Poly1305 MAC

## Notes

The API conforms to the proposed API for the CAESAR competition.

The construction conforms to the [ChaCha20 and Poly1305 for IETF protocols](http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-04) draft and Sodium's implementation is fully interoperable with other current implementations.
