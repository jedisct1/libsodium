#! /bin/sh

BUILD_ROOT=builds/particle
INCLUDE_DIR=src/libsodium/include

# Build the Particle library structure
[ -d  "$BUILD_ROOT" ] && rm -rf $BUILD_ROOT
mkdir $BUILD_ROOT
mkdir $BUILD_ROOT/examples

# Copy over the libsodium code
cp -r src $BUILD_ROOT/.

# Build version.h
sed -i '' 's/@VERSION@/1.0.18/g; s/@SODIUM_LIBRARY_VERSION_MAJOR@/10/g; s/@SODIUM_LIBRARY_VERSION_MINOR@/3/g; s/@SODIUM_LIBRARY_MINIMAL_DEF@//g' $BUILD_ROOT/$INCLUDE_DIR/sodium/version.h.in
mv $BUILD_ROOT/$INCLUDE_DIR/sodium/version.h.in $BUILD_ROOT/$INCLUDE_DIR/sodium/version.h

# Modify structure to support Particle library format
cp -r $BUILD_ROOT/$INCLUDE_DIR/*.h $BUILD_ROOT/src/.
cp -r $BUILD_ROOT/$INCLUDE_DIR/sodium/*.h $BUILD_ROOT/src/.
cp -r $BUILD_ROOT/$INCLUDE_DIR/sodium/private $BUILD_ROOT/src/.
rm -r $BUILD_ROOT/$INCLUDE_DIR
sed -i '' 's/sodium\///g' $BUILD_ROOT/src/sodium.h

# Add defines to tops of files so we don't have to use compiler directives
find $BUILD_ROOT/src -name \*.c -print -exec sed -i '' '1i\
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
' $BUILD_ROOT/src/libsodium/randombytes/randombytes.c;

# Copy our code required for the Particle port
cp -r particle/port/randombytes_particle.c $BUILD_ROOT/src/.
cp -r particle/port/randombytes_internal.h $BUILD_ROOT/src/.
cp particle/libsodium.cpp $BUILD_ROOT/src/.
cp particle/libsodium.h $BUILD_ROOT/src/.

# Copy over the the library metadata
cp LICENSE $BUILD_ROOT/.
cp README.markdown $BUILD_ROOT/README.md
cp particle/library.properties $BUILD_ROOT/.

# Clean up directory a little
find $BUILD_ROOT/src -name Makefile\* -print -exec rm {} \;