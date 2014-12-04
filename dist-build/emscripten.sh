#! /bin/sh

export MAKE_FLAGS='-j4'
export PREFIX="$(pwd)/libsodium-js"
export EXPORTED_FUNCTIONS='["_crypto_aead_chacha20poly1305_abytes","_crypto_aead_chacha20poly1305_decrypt","_crypto_aead_chacha20poly1305_encrypt","_crypto_aead_chacha20poly1305_keybytes","_crypto_aead_chacha20poly1305_npubbytes","_crypto_aead_chacha20poly1305_nsecbytes","_crypto_auth","_crypto_auth_bytes","_crypto_auth_keybytes","_crypto_auth_verify","_crypto_box_detached","_crypto_box_easy","_crypto_box_keypair","_crypto_box_macbytes","_crypto_box_noncebytes","_crypto_box_open_detached","_crypto_box_open_easy","_crypto_box_publickeybytes","_crypto_box_secretkeybytes","_crypto_box_seed_keypair","_crypto_box_seedbytes","_crypto_generichash","_crypto_generichash_bytes","_crypto_generichash_bytes_max","_crypto_generichash_bytes_min","_crypto_generichash_final","_crypto_generichash_init","_crypto_generichash_keybytes","_crypto_generichash_keybytes_max","_crypto_generichash_keybytes_min","_crypto_generichash_update","_crypto_hash","_crypto_hash_bytes","_crypto_pwhash_scryptsalsa208sha256","_crypto_pwhash_scryptsalsa208sha256_memlimit_interactive","_crypto_pwhash_scryptsalsa208sha256_memlimit_sensitive","_crypto_pwhash_scryptsalsa208sha256_opslimit_interactive","_crypto_pwhash_scryptsalsa208sha256_opslimit_sensitive","_crypto_pwhash_scryptsalsa208sha256_saltbytes","_crypto_pwhash_scryptsalsa208sha256_str","_crypto_pwhash_scryptsalsa208sha256_str_verify","_crypto_pwhash_scryptsalsa208sha256_strbytes","_crypto_pwhash_scryptsalsa208sha256_strprefix","_crypto_scalarmult","_crypto_scalarmult_base","_crypto_scalarmult_bytes","_crypto_scalarmult_scalarbytes","_crypto_secretbox_detached","_crypto_secretbox_easy","_crypto_secretbox_keybytes","_crypto_secretbox_macbytes","_crypto_secretbox_noncebytes","_crypto_secretbox_open_detached","_crypto_secretbox_open_easy","_crypto_shorthash","_crypto_shorthash_bytes","_crypto_shorthash_keybytes","_crypto_sign","_crypto_sign_bytes","_crypto_sign_detached","_crypto_sign_ed25519_pk_to_curve25519","_crypto_sign_ed25519_sk_to_curve25519","_crypto_sign_keypair","_crypto_sign_open","_crypto_sign_publickeybytes","_crypto_sign_secretkeybytes","_crypto_sign_seed_keypair","_crypto_sign_seedbytes","_crypto_sign_verify_detached","_randombytes_buf","_randombytes_close","_randombytes_random","_randombytes_set_implementation","_randombytes_stir","_randombytes_sysrandom","_randombytes_sysrandom_buf","_randombytes_sysrandom_close","_randombytes_sysrandom_stir","_randombytes_sysrandom_uniform","_randombytes_uniform","_sodium_bin2hex","_sodium_hex2bin","_sodium_init","_sodium_library_version_major","_sodium_library_version_minor","_sodium_memcmp","_sodium_memzero","_sodium_version_string"]'
export TOTAL_MEMORY=33554432
export JS_EXPORTS_FLAGS="-s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS}"
export LDFLAGS="-s TOTAL_MEMORY=${TOTAL_MEMORY}"

emconfigure ./configure --enable-minimal --disable-shared --prefix="$PREFIX" \
  CFLAGS="-O3" && \
emmake make clean && \
emmake make $MAKE_FLAGS install V=1 && \
emcc -O3 --closure 1 --llvm-lto 1 $LDFLAGS $JS_EXPORTS_FLAGS \
  "${PREFIX}/lib/libsodium.a" -o "${PREFIX}/lib/libsodium.js" && \
echo 'Compiling the test suite...' && \
emmake make $MAKE_FLAGS check > /dev/null 2>&1

echo 'Running the test suite.' && \
echo 'sodium_utils2 and sodium_utils3 are expected to fail in Javascript.' && \
(
  cd test/default && \
  for file in *.js; do
    echo "#! /usr/bin/env node" > "${file}.tmp"
    fgrep -v '#! /usr/bin/env node' "$file" >> "${file}.tmp"
    chmod +x "${file}.tmp"
    mv -f "${file}.tmp" "$file"
  done
)
make $MAKE_FLAGS check && echo 'Done.'
