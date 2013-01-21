# libsodium

[NaCl](http://nacl.cr.yp.to/) (pronounced "salt") is a new easy-to-use
high-speed software library for network communication, encryption,
decryption, signatures, etc.

NaCl's goal is to provide all of the core operations needed to build
higher-level cryptographic tools.

Sodium is a portable, cross-compilable, installable, packageable,
API-compatible version of NaCl.

## Portable

In order to pick the fastest working implementation of each primitive,
NaCl performs tests and benchmarks at compile-time. Unfortunately, the
resulting library is not garanteed to work on different hardware.

Sodium only ships portable reference implementations.

Optimized implementations (including NEON optimizations) will eventually
be supported, but tests and benchmarks will be performed at run-time,
so that the same binary package can still run everywhere.

Sodium is tested on a variety of compilers and operating systems,
including Windows, iOS and Android.

## Installable

Sodium is a shared library with a machine-independant set of
headers, so that it can easily be used by 3rd party projects.

The library is built using autotools, making it easy to package.

Installation is trivial, and both compilation and testing can take
advantage of multiple CPU cores:

    ./configure
    make && make check && make install

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
the current version of the library:

    const char *sodium_version_string(void);
    const int   sodium_version_major(void);
    const int   sodium_version_minor(void);

Headers are installed in `${prefix}/include/sodium`.

A convenience header includes everything you need to use the library:

    #include <sodium.h>

