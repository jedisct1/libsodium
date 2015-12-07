#! /bin/sh

export MAKE_FLAGS='-j4'
export PREFIX="$(pwd)/libsodium-js"
export EXPORTED_FUNCTIONS='["_crypto_auth","_crypto_auth_bytes","_crypto_auth_keybytes","_crypto_auth_verify","_crypto_box_beforenm","_crypto_box_beforenmbytes","_crypto_box_detached","_crypto_box_detached_afternm","_crypto_box_easy","_crypto_box_easy_afternm","_crypto_box_keypair","_crypto_box_macbytes","_crypto_box_noncebytes","_crypto_box_open_detached","_crypto_box_open_detached_afternm","_crypto_box_open_easy","_crypto_box_open_easy_afternm","_crypto_box_publickeybytes","_crypto_box_seal","_crypto_box_seal_open","_crypto_box_sealbytes","_crypto_box_secretkeybytes","_crypto_box_seed_keypair","_crypto_box_seedbytes","_crypto_generichash","_crypto_generichash_bytes","_crypto_generichash_bytes_max","_crypto_generichash_bytes_min","_crypto_generichash_final","_crypto_generichash_init","_crypto_generichash_keybytes","_crypto_generichash_keybytes_max","_crypto_generichash_keybytes_min","_crypto_generichash_statebytes","_crypto_generichash_update","_crypto_hash","_crypto_hash_bytes","_crypto_onetimeauth","_crypto_onetimeauth_bytes","_crypto_onetimeauth_final","_crypto_onetimeauth_init","_crypto_onetimeauth_keybytes","_crypto_onetimeauth_statebytes","_crypto_onetimeauth_update","_crypto_onetimeauth_verify","_crypto_pwhash_scryptsalsa208sha256","_crypto_pwhash_scryptsalsa208sha256_ll","_crypto_pwhash_scryptsalsa208sha256_memlimit_interactive","_crypto_pwhash_scryptsalsa208sha256_memlimit_sensitive","_crypto_pwhash_scryptsalsa208sha256_opslimit_interactive","_crypto_pwhash_scryptsalsa208sha256_opslimit_sensitive","_crypto_pwhash_scryptsalsa208sha256_saltbytes","_crypto_pwhash_scryptsalsa208sha256_str","_crypto_pwhash_scryptsalsa208sha256_str_verify","_crypto_pwhash_scryptsalsa208sha256_strbytes","_crypto_pwhash_scryptsalsa208sha256_strprefix","_crypto_scalarmult","_crypto_scalarmult_base","_crypto_scalarmult_bytes","_crypto_scalarmult_scalarbytes","_crypto_secretbox_detached","_crypto_secretbox_easy","_crypto_secretbox_keybytes","_crypto_secretbox_macbytes","_crypto_secretbox_noncebytes","_crypto_secretbox_open_detached","_crypto_secretbox_open_easy","_crypto_shorthash","_crypto_shorthash_bytes","_crypto_shorthash_keybytes","_crypto_sign","_crypto_sign_bytes","_crypto_sign_detached","_crypto_sign_ed25519_pk_to_curve25519","_crypto_sign_ed25519_sk_to_curve25519","_crypto_sign_keypair","_crypto_sign_open","_crypto_sign_publickeybytes","_crypto_sign_secretkeybytes","_crypto_sign_seed_keypair","_crypto_sign_seedbytes","_crypto_sign_verify_detached","_randombytes_buf","_randombytes_close","_randombytes_random","_randombytes_stir","_randombytes_uniform","_sodium_bin2hex","_sodium_hex2bin","_sodium_init","_sodium_library_version_major","_sodium_library_version_minor","_sodium_memzero","_sodium_version_string"]'
export TOTAL_MEMORY=33554432
export JS_EXPORTS_FLAGS="-s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS}"
export LDFLAGS="-s TOTAL_MEMORY=${TOTAL_MEMORY} -s RESERVED_FUNCTION_POINTERS=8 -s NO_BROWSER=1 -s NO_DYNAMIC_EXECUTION=1 -s RUNNING_JS_OPTS=1"

if [ "x$1" = "x--browser-tests" ]; then
  export BROWSER_TESTS='yes'
fi

if [ "x$BROWSER_TESTS" != "x" ]; then
  echo "Tests will be built to be run in a web browser"
  rm -f test/browser-js.done
else
  rm -f test/js.done
fi

emconfigure ./configure --enable-minimal --disable-shared --prefix="$PREFIX" \
  CFLAGS="-O3" && \
emmake make clean && \
emmake make $MAKE_FLAGS install V=1 && \
emcc -O3 --llvm-lto 1 --memory-init-file 0 $CPPFLAGS $LDFLAGS $JS_EXPORTS_FLAGS \
  "${PREFIX}/lib/libsodium.a" -o "${PREFIX}/lib/libsodium.js" || exit 1

if test "x$NODE" = x; then
  for candidate in node nodejs; do
    case $($candidate --version 2>&1) in #(
      v*)
        NODE=$candidate
        break ;;
    esac
  done
fi

if test "x$NODE" = x; then
  echo 'node.js not found - test suite skipped' >&2
  exit 1
fi

echo "Using [${NODE}] as a Javascript runtime"

if [ "x$BROWSER_TESTS" != "x" ]; then
  echo 'Compiling the test suite for web browsers...' && \
    emmake make $MAKE_FLAGS CPPFLAGS="$CPPFLAGS -DBROWSER_TESTS=1" check \
      > /dev/null 2>&1
else
  echo 'Compiling the test suite...' && \
    emmake make $MAKE_FLAGS check > /dev/null 2>&1
fi

if [ "x$BROWSER_TESTS" != "x" ]; then
  echo 'Creating the test suite for web browsers'
  (
    cd test/default && \
    mkdir -p browser && \
    rm -f browser/tests.txt && \
    for file in *.js; do
      fgrep -v "#! /usr/bin/env {NODE}" "$file" > "browser/${file}"
      tname=$(echo "$file" | sed 's/.js$//')
      cp -f "${tname}.exp" "browser/${tname}.exp"
      sed "s/{{tname}}/${tname}/" index.html.tpl > "browser/${tname}.html"
      echo "${tname}.html" >> "browser/tests.txt"
    done
    touch -r "${PREFIX}/lib/libsodium.js" test/browser-js.done
  )
else
  echo 'Running the test suite'
  (
    cd test/default && \
    for file in *.js; do
      echo "#! /usr/bin/env ${NODE}" > "${file}.tmp"
      fgrep -v "#! /usr/bin/env {NODE}" "$file" >> "${file}.tmp"
      chmod +x "${file}.tmp"
      mv -f "${file}.tmp" "$file"
    done
  )
  make $MAKE_FLAGS check || exit 1
  touch -r "${PREFIX}/lib/libsodium.js" test/js.done
fi

echo 'Done.'
