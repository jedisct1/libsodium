This directory contains scripts and files to package libsodium for .NET Core.

In .NET Core, it is customary to provide pre-compiled binaries for all platforms
as NuGet packages. The purpose of the `prepare.py` script in this directory is
to generate a `Makefile` that downloads pre-compiled libsodium binaries from a
list of sources and creates a NuGet package that can be uploaded to
[nuget.org](https://nuget.org/).


#### Sources

The list of sources is located in `prepare.py` and needs to be updated on each
libsodium release. Currently, libsodium binaries are obtained in the following
ways:

* For Windows, the binaries are taken from
  [download.libsodium.org](https://download.libsodium.org/libsodium/releases/).
* For macOS, the binaries are extracted from the
  [Homebrew libsodium bottle](https://bintray.com/homebrew/bottles/libsodium).
* For Linux, libsodium is compiled on and downloaded from the
  [openSUSE Build Service](https://build.opensuse.org/package/show/home:nsec/libsodium).


#### Metadata

The metadata for the NuGet package is located in `libsodium.nuspec`. On each
invocation, the `prepare.py` script generates a new version number and creates a
`.nuspec` file that contains the metadata from `libsodium.nuspec`, the generated
version number and the list of binaries.


#### Making a release

1. Update `_service` to reflect the current libsodium version.
2. Download `libsodium.spec` and `libsodium.changes` from
   [here](https://build.opensuse.org/package/show?project=devel%3Alibraries%3Ac_c%2B%2B&package=libsodium).
3. Upload `_service`, `libsodium.spec`, `libsodium.changes` and the current
   `libsodium-{version}.tar.gz` to openSUSE Build Service.
4. Update the libsodium version, file names and download URLs in `prepare.py`.
5. Run `./prepare.py` to generate the `Makefile` and
   `libsodium.{version}.nuspec`.
6. Run `make` to download the binaries and create `libsodium.{version}.nupkg`.
   You may need to install `unzip`, `rpm2cpio` and `dotnet-dev` first.
7. Verify that everything in the `.nupkg` file is in place.
8. Publish the release by uploading the `.nupkg` file to 
   [nuget.org](https://nuget.org/).
9. Commit the updated `prepare.py` and `version.json` to the libsodium
   repository.
