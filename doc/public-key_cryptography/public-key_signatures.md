# Public-key signatures

## Example

```c
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4

unsigned char pk[crypto_sign_PUBLICKEYBYTES];
unsigned char sk[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(pk, sk);

unsigned char sealed_message[crypto_sign_BYTES + MESSAGE_LEN];
unsigned long long sealed_message_len;

crypto_sign(sealed_message, &sealed_message_len,
            MESSAGE, MESSAGE_LEN, sk);

unsigned char unsealed_message[MESSAGE_LEN];
unsigned long long unsealed_message_len;
if (crypto_sign_open(unsealed_message, &unsealed_message_len,
                     sealed_message, sealed_message_len, pk) != 0) {
    /* Incorrect signature! */
}
```

## Purpose

In this system, a signer generates a key pair:
- a secret key, that will be used to append a seal to any number of messages
- a public key, that anybody can use to verify that the seal appended to a message was actually issued by the creator of the public key.

Verifiers need to already know and ultimately trust a public key before messages sealed using it can be verified.

*Warning:* this is different from authenticated encryption. Appending a seal does not change the content of the message itself.

## Key pair generation

```c
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
```

The `crypto_sign_keypair()` function randomly generates a secret key and a corresponding public key. The public key is put into `pk` (`crypto_sign_PUBLICKEYBYTES` bytes) and the secret key into `sk` (`crypto_sign_SECRETKEYBYTES` bytes).

```c
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed);
```

Using `crypto_sign_seed_keypair()`, the key pair can also be deterministically derived from a single key `seed` (`crypto_sign_SEEDBYTES` bytes).

## Signatures

```c
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);
```

The `crypto_sign()` function appends a seal to a message `m` whose length is `mlen` bytes, using the secret key `sk`.

The sealed message is put into `sm` and can be up to `crypto_sign_BYTES + mlen` bytes long.

The actual length of the sealed message is stored into `smlen`.

```c
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);
```

The `crypto_sign_open()` function checks that the sealed message `sm` whose length is `smlen` bytes has a valid seal for the public key `pk`.

If the seal is doesn't appear to be valid, the function returns `-1`.

On success, it puts the message with the seal removed into `m`, stores its length into `mlen` and returns `0`.

## Constants

- `crypto_sign_PUBLICKEYBYTES`
- `crypto_sign_SECRETKEYBYTES`
- `crypto_sign_BYTES`
- `crypto_sign_SEEDBYTES`

## Algorithm details

- Signature: Ed25519
