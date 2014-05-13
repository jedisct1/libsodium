[![Build Status](https://travis-ci.org/jedisct1/libsodium.png?branch=master)](https://travis-ci.org/jedisct1/libsodium?branch=master)

![libsodium](https://raw.github.com/jedisct1/libsodium/master/logo.png)
============

[NaCl](http://nacl.cr.yp.to/) (pronounced "salt") is a new easy-to-use
high-speed software library for network communication, encryption,
decryption, signatures, etc.

NaCl's goal is to provide all of the core operations needed to build
higher-level cryptographic tools.

Sodium is a portable, cross-compilable, installable, packageable
fork of NaCl (based on the latest released upstream version [nacl-20110221](http://hyperelliptic.org/nacl/nacl-20110221.tar.bz2)), with a compatible API.

The design choices, particularly in regard to the Curve25519
Diffie-Hellman function, emphasize security (whereas NIST curves
emphasize "performance" at the cost of security), and "magic
constants" in NaCl/Sodium have clear rationales.

The same cannot be said of NIST curves, where the specific origins of
certain constants are not described by the standards.

And despite the emphasis on higher security, primitives are faster
across-the-board than most implementations of the NIST standards.

## Portability

In order to pick the fastest working implementation of each primitive,
NaCl performs tests and benchmarks at compile-time. Unfortunately, the
resulting library is not guaranteed to work on different hardware.

Sodium performs tests at run-time, so that the same binary package can
still run everywhere.

Sodium is tested on a variety of compilers and operating systems,
including Windows (with MingW or Visual Studio, x86 and x64), iOS and Android.

## Installation

Sodium is a shared library with a machine-independent set of
headers, so that it can easily be used by 3rd party projects.

The library is built using autotools, making it easy to package.

Installation is trivial, and both compilation and testing can take
advantage of multiple CPU cores.

Download a
[tarball of libsodium](https://download.libsodium.org/libsodium/releases/),
then follow the ritual:

    ./configure
    make && make check && make install

Pre-compiled Win32 packages are available for download at the same
location.

Integrity of source tarballs can currently be checked using PGP or
verified DNS queries (`dig +dnssec +short txt <file>.download.libsodium.org`
returns the SHA256 of any file available for download).

## Pre-built binaries

Pre-built libraries for Visual studio 2010, 2012 and 2013, both for
x86 and x64, are available for download at
https://download.libsodium.org/libsodium/releases/ , courtesy of
Samuel Neves (@sneves).

## Bindings for other languages

* C++: [sodiumpp](https://github.com/rubendv/sodiumpp)
* Erlang: [Erlang-NaCl](https://github.com/tonyg/erlang-nacl)
* Erlang: [Salt](https://github.com/freza/salt)
* Haskell: [Saltine](https://github.com/tel/saltine)
* Idris: [Idris-Sodium](https://github.com/edwinb/sodium-idris)
* Java: [Kalium](https://github.com/abstractj/kalium)
* Java JNI: [Kalium-JNI](https://github.com/joshjdevl/kalium-jni)
* Julia: [Sodium.jl](https://github.com/amitmurthy/Sodium.jl)
* LUA: [lua-sodium](https://github.com/morfoh/lua-sodium)
* .NET: [libsodium-net](https://github.com/adamcaudill/libsodium-net)
* NodeJS: [node-sodium](https://github.com/paixaop/node-sodium)
* Objective C: [NAChloride](https://github.com/gabriel/NAChloride)
* OCaml: [ocaml-sodium](https://github.com/dsheets/ocaml-sodium)
* Perl: [Crypto-Sodium](https://github.com/mgregoro/Crypt-Sodium)
* Pharo/Squeak: [Crypto-NaCl](http://www.eighty-twenty.org/index.cgi/tech/smalltalk/nacl-for-squeak-and-pharo-20130601.html)
* PHP: [PHP-Sodium](https://github.com/alethia7/php-sodium)
* PHP: [libsodium-php](https://github.com/jedisct1/libsodium-php)
* Python: [PyNaCl](https://github.com/dstufft/pynacl)
* Python: [PySodium](https://github.com/stef/pysodium)
* Racket: part of [CRESTaceans](https://github.com/mgorlick/CRESTaceans/tree/master/bindings/libsodium)
* Ruby: [RbNaCl](https://github.com/cryptosphere/rbnacl)
* Ruby: [Sodium](https://github.com/stouset/sodium)
* Rust: [Sodium Oxide](https://github.com/dnaq/sodiumoxide)

## Comparison with vanilla NaCl

Sodium does not ship C++ bindings. These might be part of a distinct
package.

The default public-key signature system in NaCl was a prototype that
shouldn't be used any more.

Sodium ships with the SUPERCOP reference implementation of
[Ed25519](http://ed25519.cr.yp.to/), and uses this system by default
for `crypto_sign*` operations.

For backward compatibility, the previous system is still compiled in,
as `crypto_sign_edwards25519sha512batch*`.

## Additional features

The Sodium library provides some convenience functions in order to retrieve
the current version of the package and of the shared library:

    const char *sodium_version_string(void);
    const int   sodium_library_version_major(void);
    const int   sodium_library_version_minor(void);

Headers are installed in `${prefix}/include/sodium`.

A convenience header includes everything you need to use the library:

    #include <sodium.h>

This is not required, however, before any other libsodium functions, it
is recommended to call:

    sodium_init();

This will pick optimized implementations of some primitives, if they
appear to work as expected after running some tests, and these will be
used for subsequent operations.
It will also initialize the pseudorandom number generator.
This function should only be called once, and before performing any other
operations.
Doing so is required to ensure thread safety of all the functions provided by
the library.

Sodium also provides helper functions to generate random numbers,
leveraging `/dev/urandom` or `/dev/random` on *nix and the cryptographic
service provider on Windows. The interface is similar to
`arc4random(3)`. It is `fork(2)`-safe but not thread-safe. This holds
true for `crypto_sign_keypair()` and `crypto_box_keypair()` as well.

    uint32_t randombytes_random(void);

Return a random 32-bit unsigned value.

    void     randombytes_stir(void);

Generate a new key for the pseudorandom number generator. The file
descriptor for the entropy source is kept open, so that the generator
can be reseeded even in a chroot() jail.

    uint32_t randombytes_uniform(const uint32_t upper_bound);

Return a value between 0 and upper_bound using a uniform distribution.

    void     randombytes_buf(void * const buf, const size_t size);

Fill the buffer `buf` with `size` random bytes.

    int      randombytes_close(void);

Close the file descriptor or the handle for the cryptographic service
provider.

A custom implementation of these functions can be registered with
`randombytes_set_implementation()`.

In addition, Sodium provides a function to securely wipe a memory
region:

    void     sodium_memzero(void * const pnt, const size_t size);

Warning: if a region has been allocated on the heap, you still have
to make sure that it can't get swapped to disk, possibly using
`mlock(2)`.

In order to compare memory zones in constant time, Sodium provides:

    int      sodium_memcmp(const void * const b1_, const void * const b2_,
                           size_t size);

`sodium_memcmp()` returns `0` if `size` bytes at `b1_` and `b2_` are
equal, another value if they are not. Unlike `memcmp()`,
`sodium_memcmp()` cannot be used to put `b1_` and `b2_` into a defined
order.

And a convenience function for converting a binary buffer to a
hexadecimal string:

    char *   sodium_bin2hex(char * const hex, const size_t hexlen,
                            const unsigned char *bin, const size_t binlen);

Sensitive data should not be swapped out to disk, especially if swap
partitions are not encrypted. Libsodium provides the `sodium_mlock()`
function to lock pages in memory before writing sensitive content to
them:

    int      sodium_mlock(void *addr, size_t len);

Once done with these pages, they can be unlocked with
`sodium_munlock()`. This function will zero the data before unlocking
the pages.

    int      sodium_munlock(void * addr, size_t len);

## Easy interfaces to `crypto_box` and `crypto_secretbox`

`crypto_box` and `crypto_secretbox` require prepending
`crypto_box_ZEROBYTES` or `crypto_secretbox_ZEROBYTE` extra bytes to the
message, and making sure that these are all zeros.
A similar padding is required to decrypt the ciphertext. And this
padding is actually larger than the MAC size,
`crypto_box_MACBYTES`/`crypto_secretbox_MACBYTES`.

This API, as defined by NaCl, can be confusing. And while using a
larger buffer and two pointers is not an issue for native C
applications, this might not be an option when another runtime is
controlling the allocations.

Libsodium provides an easy, higher-level interface to these operations.

    int crypto_box_easy(unsigned char *c, const unsigned char *m,
                        unsigned long long mlen, const unsigned char *n,
                        const unsigned char *pk, const unsigned char *sk);

This function encrypts and authenticates a message `m` using the
sender's secret key `sk`, the receiver's public key `pk` and a nonce
`n`, which should be `crypto_box_NONCEBYTES` bytes long.
The ciphertext, including the MAC, will be copied to `c`, whose length
should be `len(m) + crypto_box_MACBYTES`, and that doesn't require to be
initialized.

    int crypto_box_open_easy(unsigned char *m, const unsigned char *c,
                             unsigned long long clen, const unsigned char *n,
                             const unsigned char *pk, const unsigned char *sk);

This function verifies and decrypts a ciphertext `c` as returned by
`crypto_box_easy()`, whose length is `clen`, using the nonce `n`, the
receiver's secret key `sk`, and the sender's public key `pk`. The
message is stored to `m`, whose length should be at least `len(c) -
crypto_box_MACBYTES` and that doesn't require to be initialized.

Similarily, secret-key authenticated encryption provide "easy" wrappers:

    int crypto_secretbox_easy(unsigned char *c, const unsigned char *m,
                              unsigned long long mlen, const unsigned char *n,
                              const unsigned char *k);

    int crypto_secretbox_open_easy(unsigned char *m, const unsigned char *c,
                                   unsigned long long clen,
                                   const unsigned char *n,
                                   const unsigned char *k);

The length of the ciphertext, which will include the MAC, is
`len(m) + crypto_secretbox_MACBYTES`, and the length of the buffer for
the decrypted message doesn't have to be more than `len(c) -
crypto_secretbox_MACBYTES`.

The "easy" interface currently requires allocations and copying, which
makes it slower than using the traditional NaCl interface. This
shouldn't make any sensible difference in most use cases, and future
versions of the library may not require extra copy operations any
more. Unless speed is absolutely critical, you are welcome to use the
"easy" interface, especially if you are new to NaCl/Sodium.

## New operations

### crypto_shorthash

A lot of applications and programming language implementations have
been recently found to be vulnerable to denial-of-service attacks when
a hash function with weak security guarantees, like Murmurhash 3, was
used to construct a hash table.

In order to address this, Sodium provides the “shorthash” function,
currently implemented using SipHash-2-4. This very fast hash function
outputs short, but unpredictable (without knowing the secret key)
values suitable for picking a list in a hash table for a given key.

See `crypto_shorthash.h` for details.

### crypto_generichash

This hash function provides:

* A variable output length (up to `crypto_generichash_BYTES_MAX` bytes)
* A variable key length (from no key at all to
  `crypto_generichash_KEYBYTES_MAX` bytes)
* A simple interface as well as a streaming interface.

`crypto_generichash` is currently being implemented using
[Blake2](https://blake2.net/).

### crypto_pwhash (scrypt)

High-level functions for password hashing are not defined yet: they will
eventually be wrappers for the winning function of the ongoing
[Password Hashing Competition](https://password-hashing.net/).

Meanwhile, the [scrypt](http://www.tarsnap.com/scrypt.html) function is
available through explicitly-named functions, and will remain available
in the library even after the PHC.

    int crypto_pwhash_scryptxsalsa208sha256(unsigned char *out,
                                            unsigned long long outlen,
                                            const char *passwd,
                                            unsigned long long passwdlen,
                                            const unsigned char *salt,
                                            unsigned long long opslimit,
                                            size_t memlimit);

This function derives `outlen` bytes from a password `passwd` and a
salt `salt` that has to be `crypto_pwhash_scryptxsalsa208sha256_SALTBYTES`
bytes long.

The function will use at most `memlimit` bytes of memory and `opslimit`
is the maximum number of iterations to perform. Making the function
memory-hard and CPU intensive by increasing these parameters might increase
security.

Although password storage was not the primary goal of the scrypt
function, it can still be used for this purpose:

    int crypto_pwhash_scryptxsalsa208sha256_str
        (char out[crypto_pwhash_scryptxsalsa208sha256_STRBYTES],
         const char *passwd,
         unsigned long long passwdlen,
         unsigned long long opslimit,
         size_t memlimit);

This function returns a `crypto_pwhash_scryptxsalsa208sha256_STRBYTES`
bytes C string (the length includes the final `\0`) suitable for storage.
The string is guaranteed to only include ASCII characters.

The function will use at most `memlimit` bytes of memory and `opslimit`
is the maximum number of iterations to perform. These parameters are
included in the output string, and do not need to be stored separately.

The function automatically generates a random salt, which is also
included in the output string.

    int crypto_pwhash_scryptxsalsa208sha256_str_verify
        (const char str[crypto_pwhash_scryptxsalsa208sha256_STRBYTES],
         const char *passwd,
         unsigned long long passwdlen);

This function verifies that hashing the plaintext password `passwd`
results in the stored hash value included in `str` when using the same
parameters.

`0` is returned if the passwords are matching, `-1` is they are not.
The plaintext password should be locked in memory using
`sodium_mlock()` and immediately zeroed out and unlocked after this
function returns, using `sodium_munlock()`.

## Constants available as functions

In addition to constants for key sizes, output sizes and block sizes,
Sodium provides these values through function calls, so that using
them from different languages is easier.

## CurveCP

CurveCP tools are part of a different project,
[libchloride](https://github.com/jedisct1/libchloride).
If you are interested in an embeddable CurveCP implementation, take a
look at [libcurvecpr](https://github.com/impl/libcurvecpr).

## Mailing list

A mailing-list is available to discuss libsodium.

In order to join, just send a random mail to `sodium-subscribe` {at}
`pureftpd`{dot}`org`.

## License

[ISC license](http://en.wikipedia.org/wiki/ISC_license).

See the `COPYING` file for details, `AUTHORS` for designers and
implementors, and `THANKS` for contributors.
