# libsodium - P(ortable|ackageable) NaCl

Portable, cross-compilable, installable, packageable version of the NaCl
crypto library.

## differences from the vanilla NaCl

We are using ed25519 from SUPERCOP, as even DJB doesn't recommend using edwards25519sha512batch anymore - this will reach NaCL one day...
