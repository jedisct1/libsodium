[![Build Status](https://travis-ci.org/jedisct1/libsodium.png?branch=master)](https://travis-ci.org/jedisct1/libsodium?branch=master)

![libsodium](https://raw.github.com/jedisct1/libsodium/master/logo.png)
============

[NaCl](http://nacl.cr.yp.to/) (pronounced "salt") is a new easy-to-use
high-speed software library for network communication, encryption,
decryption, signatures, etc.

NaCl's goal is to provide all of the core operations needed to build
higher-level cryptographic tools.

Sodium is a portable, cross-compilable, installable, packageable
crypto library based on NaCl, with a compatible API.

## Portability

In order to pick the fastest working implementation of each primitive,
NaCl performs tests and benchmarks at compile-time. Unfortunately, the
resulting library is not guaranteed to work on different hardware.

Sodium performs tests at run-time, so that the same binary package can
still run everywhere.

Sodium is tested on a variety of compilers and operating systems,
including Windows, iOS and Android.

## Installation

Sodium is a shared library with a machine-independent set of
headers, so that it can easily be used by 3rd party projects.

The library is built using autotools, making it easy to package.

Installation is trivial, and both compilation and testing can take
advantage of multiple CPU cores.

Download a
[tarball of libsodium](http://download.dnscrypt.org/libsodium/releases/),
then follow the ritual:

    ./configure
    make && make check && make install

Pre-compiled Win32 packages are available for download at the same
location.

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

This is not required, however, before any other libsodium function, you can
call:

    sodium_init();

This will pick optimized implementations of some primitives, if they
appear to work as expected after running some tests, and these will be
used for subsequent operations. It only need to be called once.

This function is not thread-safe. No other Sodium functions should be
called until it successfully returns. In a multithreading environment,
if, for some reason, you really need to call `sodium_init()` while some
other Sodium functions may be running in different threads, add locks
accordingly (both around `sodium_init()` and around other functions).

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

In order to compare memory zones in constant time, Sodium proides:

    int      sodium_memcmp(const void * const b1_, const void * const b2_,
                           size_t size);

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

## Constants available as functions

In addition to constants for key sizes, output sizes and block sizes,
Sodium provides these values through function calls, so that using
them from different languages is easier.

## Bindings for other languages

* Erlang: [Erlang-NaCl](https://github.com/tonyg/erlang-nacl)
* Java: [Kalium](https://github.com/abstractj/kalium)
* Julia: [Sodium.jl](https://github.com/amitmurthy/Sodium.jl)
* Python: [PyNaCl](https://github.com/dstufft/pynacl)
* Racket: part of [CRESTaceans](https://github.com/mgorlick/CRESTaceans/tree/master/bindings/libsodium)
* Ruby: [RbNaCl](https://github.com/cryptosphere/rbnacl)
* Ruby: [Sodium](https://github.com/stouset/sodium)

## CurveCP

CurveCP tools are part of a different project,
[libchloride](https://github.com/jedisct1/libchloride).
If you are interested in an embeddable CurveCP implementation, take a
look at [libcurvecpr](https://github.com/impl/libcurvecpr).

## Mailing list

A mailing-list is available to discuss libsodium.

In order to join, just send a random mail to `sodium-subscribe` {at}
`pureftpd`{dot}`org`.
