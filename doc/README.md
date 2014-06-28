## Introduction

[NaCl](http://nacl.cr.yp.to/) (pronounced "salt") is an easy-to-use
high-speed software library for network communication, encryption,
decryption, signatures, etc. NaCl's goal is to provide all of the core operations needed to build higher-level cryptographic tools.

Sodium is a portable, cross-compilable, installable, packageable
fork of NaCl, with a compatible API.

The design choices emphasize security, and "magic
constants" have clear rationales.

The same cannot be said of NIST curves, where the specific origins of
certain constants are not described by the standards.

And despite the emphasis on higher security, primitives are faster
across-the-board than most implementations of the NIST standards.

Sodium supports a variety of compilers and operating systems, including Windows (with MingW or Visual Studio, x86 and x64), iOS and Android.

### Mailing list

A mailing-list is available to discuss libsodium.

In order to join, just send a random mail to `sodium-subscribe` {at}
`pureftpd`{dot}`org`.

### License

[ISC license](http://en.wikipedia.org/wiki/ISC_license).

See the `COPYING` file for details.
