#! /bin/sh

# Build the Particle library structure
[ -d "builds/particle" ] && rm -rf builds/particle
mkdir builds/particle
mkdir builds/particle/examples
mkdir builds/particle/src

# Copy over the libsodium code
cp -r src/libsodium builds/particle/src/.

# Build version.h
sed -i '' 's/@VERSION@/1.0.18/g' builds/particle/src/libsodium/include/sodium/version.h.in
sed -i '' 's/@SODIUM_LIBRARY_VERSION_MAJOR@/10/g' builds/particle/src/libsodium/include/sodium/version.h.in
sed -i '' 's/@SODIUM_LIBRARY_VERSION_MINOR@/3/g' builds/particle/src/libsodium/include/sodium/version.h.in
sed -i '' 's/@SODIUM_LIBRARY_MINIMAL_DEF@//g' builds/particle/src/libsodium/include/sodium/version.h.in
mv builds/particle/src/libsodium/include/sodium/version.h.in builds/particle/src/libsodium/include/sodium/version.h

# Modify structure to support Particle library format
cp -r builds/particle/src/libsodium/include/*.h builds/particle/src/.
cp -r builds/particle/src/libsodium/include/sodium/*.h builds/particle/src/.
cp -r builds/particle/src/libsodium/include/sodium/private builds/particle/src/.
# cp builds/particle/inc/sodium/private/*.h builds/particle/inc/.
rm -r builds/particle/src/libsodium/include
sed -i '' 's/sodium\///g' builds/particle/src/sodium.h

# Add defines to tops of files so we don't have to use compiler directives
find ./builds/particle/src -name \*.c -print -exec sed -i '' '1i\
/*** Defines added by Particle build script ***/ \
// Prevent warnings associated with building without running autogen.sh \
#define CONFIGURED 1 \
\
// Compiler directives \
#define HAVE_WEAK_SYMBOLS 1 \
#define __STDC_LIMIT_MACROS \
#define __STDC_CONSTANT_MACROS \
/*** End Particle build script defines ***/ \
\
' {} \;

sed -i '' '1i\
#define RANDOMBYTES_DEFAULT_IMPLEMENTATION
' builds/particle/src/libsodium/randombytes/randombytes.c;

# Copy our code required for the Particle port
cp -r particle/port/randombytes_particle.c builds/particle/src/.
cp -r particle/port/randombytes_internal.h builds/particle/src/.
cp particle/libsodium.cpp builds/particle/src/.
cp particle/libsodium.h builds/particle/src/.

# Copy over the the library metadata
cp LICENSE builds/particle/.
cp README.markdown builds/particle/README.md
cp particle/library.properties builds/particle/.