# Secret-key authentication

## Example

```c
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4

unsigned char key[crypto_auth_KEYBYTES];
unsigned char mac[crypto_auth_BYTES];

randombytes_buf(key, sizeof key);
crypto_auth(mac, MESSAGE, MESSAGE_LEN, key);

if (crypto_auth_verify(mac, MESSAGE, MESSAGE_LEN, key) != 0) {
    /* message forged! */
}
```

## Purpose

This operation computes an authentication tag for a message and a secret key, and provides a way to verify that a given tag is valid for a given message and a key.

The function computing the tag deterministic: the same (message, key) tuple will always produce the same output.

However, even if the message is public, knowing the key is required in order to be able to compute a valid tag. Therefore, the key should remain confidential. The tag, however, can be public.

A typical use case is:
- `A` prepares a message, add an authentication tag, sends it to `B`
- `A` doesn't store the message
- Later on, `B` sends the message and the authentication tag to `A`
- `A` uses the authentication tag to verify that it created this message.

This operation does *not* encrypt the message. It only computes and verifies an authentication tag.

## Usage

```c
int crypto_auth(unsigned char *out, const unsigned char *in,
                unsigned long long inlen, const unsigned char *k);
```

The `crypto_auth()` function computes a tag for the message `in`, whose length is `inlen` bytes, and the key `k`.
`k` should be `crypto_auth_KEYBYTES` bytes.
The function puts the tag into `out`. The tag is `crypto_auth_BYTES` bytes long.

```c
int crypto_auth_verify(const unsigned char *h, const unsigned char *in,
                       unsigned long long inlen, const unsigned char *k);
```

The `crypto_auth_verify()` function verifies that the tag stored at `h` is a valid tag for the message `in` whose length is `inlen` bytes, and the key `k`.

It returns `-1` if the verification fails, and `0` if it passes.

## Constants

- `crypto_auth_BYTES`
- `crypto_auth_KEYBYTES`

## Algorithm details

- HMAC-SHA512256
