#!/bin/sh

# uncomment below to disable the minimal build
#export LIBSODIUM_FULL_BUILD=TRUE
# uncomment below and change the install location(s) to where you keep your ndk libraries and includes
#export PREFIX=/usr/local
# to override default behaviour uncomment one of the two below lines. if set LIB_PATH will be a specific path where library files
# will be stored. if set LIB_PATH_PREFIX will have a folder appended to it based on the target ABI for the specific compile.
#export LIB_PATH=${PREFIX}/lib
#export LIB_PATH_PREFIX=${PREFIX}/lib
#export INCLUDE_PATH=${PREFIX}/include

"$(dirname "$0")/android-armv7-a.sh"
"$(dirname "$0")/android-armv8-a.sh"
"$(dirname "$0")/android-x86.sh"
"$(dirname "$0")/android-x86_64.sh"

echo "
Android libraries have been generated
to use them with CMAKE you can add the following
to your CMakeLists.txt file:

list(APPEND CMAKE_FIND_ROOT_PATH "${PREFIX}")
find_library(libsodium NAMES sodium REQUIRED)

and then add libsodium to your target_link_libraries.

for static linking replace the find_library line with:

if (${ANDROID_ABI} STREQUAL "x86")

    find_library(added-salt NAMES "libsodium.so"
            REQUIRED)

else ()

    find_library(added-salt NAMES "libsodium.a"
            REQUIRED)

endif ()

Due to text relocation rules static linking 
does not work with x86 targets
"
