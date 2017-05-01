#! /bin/sh


#Preparing the enviroment
echo "-----------------------------------------------------------------"
echo "---> In order to test if you have all the packagues needed to compile, the script needs your user password:"
echo "-----------------------------------------------------------------"
sudo dpkg-query -W git nodejs python2.7 build-essential cmake default-jre tar libtool autotools-dev automake 2>/dev/null 1>/dev/null
RESULT=$?
if [ $RESULT -eq 1 ]; then
    echo ""
    echo ""
    echo ""
    echo ""
    echo "-----------------------------------------------------------------"
    echo "--- Packagues needed to compile..."
    echo "-----------------------------------------------------------------"
    echo ""
    echo ""
    echo ""
    echo ""
    sudo apt-get update
    sudo apt-get install git nodejs python2.7 build-essential cmake default-jre tar libtool autotools-dev automake
fi


#Preparing the LibSodium
if [ ! -d "./libsodium-js" ]; then
    echo ""
    echo ""
    echo ""
    echo ""
    echo "-----------------------------------------------------------------"
    echo "--- Autogen, configure and make Libsodium..."
    echo "-----------------------------------------------------------------"
    echo ""
    echo ""
    echo ""
    echo ""
    ./autogen.sh
    ./configure
    make clean
    make
fi


#Obtaining and configuring emsdk
directory=`pwd`
export PATH="$PATH:$directory/emsdk:$directory/emsdk/clang/fastcomp/build_master_64/bin:$directory/emsdk/emscripten/master"
if [ ! -d "./emsdk" ]; then
    echo ""
    echo ""
    echo ""
    echo ""
    echo "-----------------------------------------------------------------"
    echo "--- Downloading, updating, installing and activating emsdk..."
    echo "-----------------------------------------------------------------"
    echo ""
    echo ""
    echo ""
    echo ""
    rm ./emsdk-portable.tar.gz
    wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
    tar -xzvf ./emsdk-portable.tar.gz
    mv ./emsdk_portable ./emsdk
    # Fetch the latest registry of available tools.
    ./emsdk/emsdk update
    # Download and install the latest SDK tools.
    ./emsdk/emsdk install latest
    # Make the "latest" SDK "active"
    ./emsdk/emsdk activate latest
    # Set the current Emscripten path on Linux/Mac OS X
    source ./emsdk/emsdk_env.sh
    # Configure nodejs instead of node
    rm ~/.emscripten
    echo "import os" >> ~/.emscripten
    echo "SPIDERMONKEY_ENGINE = \"\"" >> ~/.emscripten
    echo "NODE_JS = \"nodejs\"" >> ~/.emscripten
    echo "LLVM_ROOT=\"$directory/emsdk/clang/fastcomp/build_master_64/bin\"" >> ~/.emscripten
    echo "EMSCRIPTEN_ROOT=\"$directory/emsdk/emscripten/master\"" >> ~/.emscripten
    echo "V8_ENGINE = \"\"" >> ~/.emscripten
    echo "TEMP_DIR = \"/tmp\"" >> ~/.emscripten
    echo "COMPILER_ENGINE = NODE_JS" >> ~/.emscripten
    echo "JS_ENGINES = [NODE_JS]" >> ~/.emscripten
    echo "" >> ~/.emscripten
    # Eliminating the default emcc header....
    sed '/if (!Module) Module = (typeof {{{ EXPORT_NAME }}} !== '\''undefined'\'' ? {{{ EXPORT_NAME }}} : null) || {};/d' ./emsdk/emscripten/master/src/shell.js > ./emscripten/master/src/shell_new.js
    mv ./emsdk/emscripten/master/src/shell_new.js ./emsdk/emscripten/master/src/shell.js
fi


#Making the real conversion
echo ""
echo ""
echo ""
echo ""
echo "-----------------------------------------------------------------"
echo "--- Doing the real conversion Libsodium C ---> Libsodium JS..."
echo "-----------------------------------------------------------------"
echo ""
echo ""
echo ""
echo ""
export PREFIX="$(pwd)/libsodium-js"
export EXPORTED_FUNCTIONS='["_crypto_generichash_blake2b_state","_crypto_generichash_state","_crypto_sign_ed25519","_crypto_box_curve25519xsalsa20poly1305_open","_crypto_box_curve25519xsalsa20poly1305","_crypto_pwhash_scryptsalsa208sha256","_crypto_hash_sha512","_crypto_scalarmult_curve25519_base","_crypto_stream_xor","_crypto_pwhash_scryptsalsa208sha256_ll","_crypto_stream","_crypto_aead_chacha20poly1305_abytes","_crypto_aead_chacha20poly1305_decrypt","_crypto_aead_chacha20poly1305_encrypt","_crypto_aead_chacha20poly1305_keybytes","_crypto_aead_chacha20poly1305_npubbytes","_crypto_aead_chacha20poly1305_nsecbytes","_crypto_auth","_crypto_auth_bytes","_crypto_auth_keybytes","_crypto_auth_verify","_crypto_box","_crypto_box_detached","_crypto_box_easy","_crypto_box_keypair","_crypto_box_macbytes","_crypto_box_noncebytes","_crypto_box_open","_crypto_box_open_detached","_crypto_box_open_easy","_crypto_box_publickeybytes","_crypto_box_secretkeybytes","_crypto_box_seed_keypair","_crypto_box_seedbytes","_crypto_generichash","_crypto_generichash_bytes","_crypto_generichash_bytes_max","_crypto_generichash_bytes_min","_crypto_generichash_final","_crypto_generichash_init","_crypto_generichash_keybytes","_crypto_generichash_keybytes_max","_crypto_generichash_keybytes_min","_crypto_generichash_update","_crypto_hash","_crypto_hash_bytes","_crypto_scalarmult","_crypto_scalarmult_base","_crypto_scalarmult_bytes","_crypto_scalarmult_scalarbytes","_crypto_secretbox","_crypto_secretbox_detached","_crypto_secretbox_easy","_crypto_secretbox_keybytes","_crypto_secretbox_macbytes","_crypto_secretbox_noncebytes","_crypto_secretbox_open","_crypto_secretbox_open_detached","_crypto_secretbox_open_easy","_crypto_shorthash","_crypto_shorthash_bytes","_crypto_shorthash_keybytes","_crypto_sign","_crypto_sign_bytes","_crypto_sign_detached","_crypto_sign_ed25519_pk_to_curve25519","_crypto_sign_ed25519_sk_to_curve25519","_crypto_sign_keypair","_crypto_sign_open","_crypto_sign_publickeybytes","_crypto_sign_secretkeybytes","_crypto_sign_seed_keypair","_crypto_sign_seedbytes","_crypto_sign_verify_detached","_randombytes","_randombytes_buf","_randombytes_close","_randombytes_random","_randombytes_set_implementation","_randombytes_stir","_randombytes_sysrandom","_randombytes_sysrandom_buf","_randombytes_sysrandom_close","_randombytes_sysrandom_stir","_randombytes_sysrandom_uniform","_randombytes_uniform","_sodium_bin2hex","_sodium_hex2bin","_sodium_init","_sodium_library_version_major","_sodium_library_version_minor","_sodium_memcmp","_sodium_memzero","_sodium_version_string"]'
export TOTAL_MEMORY=33554432
export OPTFLAGS="--llvm-lto 1 -O1 -g0" #If you try to optimize more the code, some functions will break, like crypto_sign_verify_detached
#export OPTFLAGS="--llvm-lto 0 -O0 -g3 --emit-symbol-map" #For debug
export CFLAGS="$OPTFLAGS --pre-js ${PREFIX}/../dist-build/jsInsert/libsodium_prefix.js --post-js ${PREFIX}/../dist-build/jsInsert/libsodium_suffix.js"
export LDFLAGS="-s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} -s TOTAL_MEMORY=${TOTAL_MEMORY}"

emconfigure ./configure --disable-shared --prefix="$PREFIX"
emmake make clean
emmake make install
emcc --closure 0 $CFLAGS $LDFLAGS "${PREFIX}/lib/libsodium.a" -o "${PREFIX}/lib/libsodium.js"

firefox ./dist-build/jsInsert/test.html



