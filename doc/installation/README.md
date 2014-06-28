# Installation

Sodium is a shared library with a machine-independent set of
headers, so that it can easily be used by 3rd party projects.

The library is built using autotools, making it easy to package.

Installation is trivial, and both compilation and testing can take
advantage of multiple CPU cores.

Download a
[tarball of libsodium](https://download.libsodium.org/libsodium/releases/),
then follow the ritual:

```bash
$ ./configure
$ make && make check
# make install
```

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
