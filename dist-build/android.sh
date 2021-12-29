#!/bin/sh

#uncomment below and change the install location to where you keep your ndk libraries and includes
#export PREFIX=/some/custom/ndkdeps/dir/

"$(dirname "$0")/android-armv7-a.sh"
"$(dirname "$0")/android-armv8-a.sh"
"$(dirname "$0")/android-x86.sh"
"$(dirname "$0")/android-x86_64.sh"
