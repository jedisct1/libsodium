[![GitHub CI](https://github.com/jedisct1/libsodium/workflows/CI/badge.svg)](https://github.com/jedisct1/libsodium/actions)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/2397/badge.svg)](https://scan.coverity.com/projects/2397)
[![Azure build status](https://jedisct1.visualstudio.com/Libsodium/_apis/build/status/jedisct1.libsodium?branchName=stable)](https://jedisct1.visualstudio.com/Libsodium/_build/latest?definitionId=3&branchName=stable)
[![CodeQL scan](https://github.com/jedisct1/libsodium/workflows/CodeQL%20scan/badge.svg)](https://github.com/jedisct1/libsodium/actions)

# ![libsodium](https://raw.github.com/jedisct1/libsodium/master/logo.png)

Sodium is an easy-to-use software library that provides a wide range of cryptographic operations including encryption, decryption, digital signatures, and secure password hashing.

It is a portable, cross-compilable, installable, and packageable fork of [NaCl](http://nacl.cr.yp.to/). While maintaining API compatibility, libsodium extends functionality to improve usability and simplify the development of secure applications.

---

## Key Features

- **Encryption & Decryption:** Securely encrypt and decrypt data with modern algorithms.
- **Digital Signatures:** Create and verify signatures to ensure data authenticity.
- **Cross-Platform Compatibility:** Supported on Windows (MinGW and Visual Studio, x86, x64 and arm64), iOS, Android, JavaScript, and WebAssembly.
- **User-Friendly API:** Designed to provide all core cryptographic operations while remaining easy to integrate into your projects.

---

## Documentation

- [Installation](https://doc.libsodium.org/installation)
- [Quickstart](https://doc.libsodium.org/quickstart)
- [Full Documentation](https://doc.libsodium.org)
- [Releases](https://download.libsodium.org/libsodium/releases/)
- [Integrity Checking](https://doc.libsodium.org/installation#integrity-checking)

---

## Versioning

libsodium uses a two-tier release system:

- **Point releases** (e.g., 1.0.19, 1.0.20, 1.0.21) are tagged when new features are added or significant changes are made.
- **Stable releases** are frequent maintainance updates between point releases. They fix minor issues while remaining fully compatible with their parent point release. No new features, no breaking changes.

If your application depends on a specific point release, stable updates are safe to apply. Security fixes go to the `stable` branch immediately, with a new point release tagged shortly after.

---

## Contributors

### Code Contributors

This project thrives thanks to the valuable contributions from our community. View all the [contributors](https://github.com/jedisct1/libsodium/graphs/contributors):

<a href="https://github.com/jedisct1/libsodium/graphs/contributors">
  <img src="https://opencollective.com/libsodium/contributors.svg?width=890&button=false" alt="Contributors">
</a>

### Financial Contributors

Your financial support helps us sustain and further develop libsodium.

- [Become a Financial Contributor](https://opencollective.com/libsodium/contribute)

#### Individuals

<a href="https://opencollective.com/libsodium">
  <img src="https://opencollective.com/libsodium/individuals.svg?width=890" alt="Individual Contributors">
</a>

#### Organizations

Support libsodium with your organization and gain visibility through your logo and website link.

- [Support with Your Organization](https://opencollective.com/libsodium/contribute)

<a href="https://opencollective.com/libsodium/organization/0/website">
  <img src="https://opencollective.com/libsodium/organization/0/avatar.svg" alt="Organization Contributor">
</a>

---

## License

This project is distributed under the [ISC license](https://en.wikipedia.org/wiki/ISC_license).
