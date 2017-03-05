This directory contains scripts and files to package libsodium for .NET Core.

In .NET Core, it is customary to provide pre-compiled binaries for all platforms
as NuGet packages. The purpose of the `prepare.py` script in this directory is
to generate a `Makefile` that downloads and builds libsodium binaries for a
number of platforms and assembles them in a NuGet package that can be uploaded
to [nuget.org](https://nuget.org/).

* For Windows, binaries are obtained from
  [download.libsodium.org](https://download.libsodium.org/libsodium/releases/).
* For macOS, binaries are extracted from the
  [Homebrew libsodium bottle](https://bintray.com/homebrew/bottles/libsodium).
* For Linux, libsodium is compiled in Docker containers.

See `prepare.py` for the complete list of supported platforms.

The metadata for the NuGet package is located in `libsodium.props`.


**Making a pre-release**

1. Run `python3 prepare.py 1.0.11-preview-01` to generate the `Makefile`.
   `1.0.11` is the libsodium version number; `01` is the pre-release
   number and needs to be incremented for each pre-release.
2. Take a look at the generated `Makefile`. It uses `sudo` a few times.
3. Run `make` to download and build the binaries and create the NuGet
   package. You may need to install `docker`, `make`, `curl`, `tar` and
   `unzip` first. The NuGet package is output as a `.nupkg` file in the
   `build` directory.
4. Grab a cup of coffee. Downloading the Docker images and compiling the
   Linux binaries takes a while.
5. Verify that everything in the `.nupkg` file is in place.
6. Publish the release by uploading the `.nupkg` file to 
   [nuget.org](https://nuget.org/).


**Making a release**

1. Run `python3 prepare.py 1.0.11` to generate the `Makefile`.

The remaining steps are the same.
