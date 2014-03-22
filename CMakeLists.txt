
project(sodium)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0028 NEW)
enable_testing()


#SODIUM_STATIC

INCLUDE(TestBigEndian)

TEST_BIG_ENDIAN(BE)
if(${BE})
  ADD_DEFINITIONS("-DNATIVE_BIG_ENDIAN=1")
else()
  ADD_DEFINITIONS("-DNATIVE_LITTLE_ENDIAN=1")
endif()

CONFIGURE_FILE(
  src/libsodium/include/sodium/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/sodium/version.h
  )

add_library(sodium
  src/libsodium/crypto_box/crypto_box.c
  src/libsodium/crypto_box/curve25519xsalsa20poly1305/box_curve25519xsalsa20poly1305_api.c
  src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/keypair_curve25519xsalsa20poly1305.c
  src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/before_curve25519xsalsa20poly1305.c
  src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/after_curve25519xsalsa20poly1305.c
  src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/box_curve25519xsalsa20poly1305.c
  src/libsodium/crypto_box/crypto_box_easy.c
  src/libsodium/crypto_sign/crypto_sign.c
  src/libsodium/crypto_sign/edwards25519sha512batch/sign_edwards25519sha512batch_api.c
  src/libsodium/crypto_sign/edwards25519sha512batch/ref/fe25519_edwards25519sha512batch.c
  src/libsodium/crypto_sign/edwards25519sha512batch/ref/sign_edwards25519sha512batch.c
  src/libsodium/crypto_sign/edwards25519sha512batch/ref/ge25519_edwards25519sha512batch.c
  src/libsodium/crypto_sign/edwards25519sha512batch/ref/sc25519_edwards25519sha512batch.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_0.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_sq2.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_sub.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p1p1_to_p2.c
  src/libsodium/crypto_sign/ed25519/ref10/open.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_invert.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_double_scalarmult.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_mul.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_add.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p2_0.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p3_0.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_tobytes.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_frombytes.c
  src/libsodium/crypto_sign/ed25519/ref10/sc_reduce.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_cmov.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p3_to_p2.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_add.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_madd.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_frombytes.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_scalarmult_base.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_copy.c
  src/libsodium/crypto_sign/ed25519/ref10/keypair.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_isnegative.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_1.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p3_tobytes.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_sq.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_pow22523.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_msub.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p2_dbl.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p3_dbl.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p1p1_to_p3.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_isnonzero.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_tobytes.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_p3_to_cached.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_sub.c
  src/libsodium/crypto_sign/ed25519/ref10/fe_neg.c
  src/libsodium/crypto_sign/ed25519/ref10/ge_precomp_0.c
  src/libsodium/crypto_sign/ed25519/ref10/sign.c
  src/libsodium/crypto_sign/ed25519/ref10/sc_muladd.c
  src/libsodium/crypto_sign/ed25519/sign_ed25519_api.c
  src/libsodium/crypto_hash/sha512/ref/hash_sha512.c
  src/libsodium/crypto_hash/sha512/hash_sha512_api.c
  src/libsodium/crypto_hash/sha256/ref/hash_sha256.c
  src/libsodium/crypto_hash/sha256/hash_sha256_api.c
  src/libsodium/crypto_hash/crypto_hash.c
  src/libsodium/sodium/utils.c
  src/libsodium/sodium/version.c
  src/libsodium/sodium/core.c
  src/libsodium/sodium/compat.c
  src/libsodium/crypto_onetimeauth/poly1305/donna/auth_poly1305_donna.c
  src/libsodium/crypto_onetimeauth/poly1305/donna/verify_poly1305_donna.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_api.c
  src/libsodium/crypto_onetimeauth/poly1305/53/auth_poly1305_53.c
  src/libsodium/crypto_onetimeauth/poly1305/53/verify_poly1305_53.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_try.c
  src/libsodium/crypto_onetimeauth/crypto_onetimeauth.c
  src/libsodium/crypto_auth/crypto_auth.c
  src/libsodium/crypto_auth/hmacsha256/ref/hmac_hmacsha256.c
  src/libsodium/crypto_auth/hmacsha256/ref/verify_hmacsha256.c
  src/libsodium/crypto_auth/hmacsha256/auth_hmacsha256_api.c
  src/libsodium/crypto_auth/hmacsha512256/ref/hmac_hmacsha512256.c
  src/libsodium/crypto_auth/hmacsha512256/ref/verify_hmacsha512256.c
  src/libsodium/crypto_auth/hmacsha512256/auth_hmacsha512256_api.c
  src/libsodium/crypto_shorthash/crypto_shorthash.c
  src/libsodium/crypto_shorthash/siphash24/shorthash_siphash24_api.c
  src/libsodium/crypto_shorthash/siphash24/ref/shorthash_siphash24.c
  src/libsodium/crypto_generichash/crypto_generichash.c
  src/libsodium/crypto_generichash/blake2/generichash_blake2_api.c
  src/libsodium/crypto_generichash/blake2/ref/blake2b-ref.c
  src/libsodium/crypto_generichash/blake2/ref/generichash_blake2b.c
  src/libsodium/crypto_generichash/blake2/ref/blake2s-ref.c
  src/libsodium/crypto_scalarmult/crypto_scalarmult.c
  src/libsodium/crypto_scalarmult/curve25519/ref/base_curve25519_ref.c
  src/libsodium/crypto_scalarmult/curve25519/ref/smult_curve25519_ref.c
  src/libsodium/crypto_scalarmult/curve25519/donna_c64/base_curve25519_donna_c64.c
  src/libsodium/crypto_scalarmult/curve25519/donna_c64/smult_curve25519_donna_c64.c
  src/libsodium/crypto_scalarmult/curve25519/scalarmult_curve25519_api.c
  src/libsodium/randombytes/randombytes.c
  src/libsodium/randombytes/salsa20/randombytes_salsa20_random.c
  src/libsodium/randombytes/sysrandom/randombytes_sysrandom.c
  src/libsodium/crypto_stream/salsa2012/stream_salsa2012_api.c
  src/libsodium/crypto_stream/salsa2012/ref/stream_salsa2012.c
  src/libsodium/crypto_stream/salsa2012/ref/xor_salsa2012.c
  src/libsodium/crypto_stream/aes256estream/stream_aes256estream_api.c
  src/libsodium/crypto_stream/aes256estream/hongjun/aes256-ctr.c
  src/libsodium/crypto_stream/xsalsa20/ref/xor_xsalsa20.c
  src/libsodium/crypto_stream/xsalsa20/ref/stream_xsalsa20.c
  src/libsodium/crypto_stream/xsalsa20/stream_xsalsa20_api.c
  src/libsodium/crypto_stream/salsa20/stream_salsa20_api.c
  src/libsodium/crypto_stream/salsa20/ref/stream_salsa20_ref.c
  src/libsodium/crypto_stream/salsa20/ref/xor_salsa20_ref.c
  src/libsodium/crypto_stream/aes128ctr/portable/int128_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/afternm_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/xor_afternm_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/stream_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/beforenm_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/consts_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/portable/common_aes128ctr.c
  src/libsodium/crypto_stream/aes128ctr/stream_aes128ctr_api.c
  src/libsodium/crypto_stream/salsa208/stream_salsa208_api.c
  src/libsodium/crypto_stream/salsa208/ref/xor_salsa208.c
  src/libsodium/crypto_stream/salsa208/ref/stream_salsa208.c
  src/libsodium/crypto_stream/crypto_stream.c
  src/libsodium/crypto_secretbox/crypto_secretbox_easy.c
  src/libsodium/crypto_secretbox/crypto_secretbox.c
  src/libsodium/crypto_secretbox/xsalsa20poly1305/ref/box_xsalsa20poly1305.c
  src/libsodium/crypto_secretbox/xsalsa20poly1305/secretbox_xsalsa20poly1305_api.c
  src/libsodium/crypto_core/salsa2012/core_salsa2012_api.c
  src/libsodium/crypto_core/salsa2012/ref/core_salsa2012.c
  src/libsodium/crypto_core/salsa20/core_salsa20_api.c
  src/libsodium/crypto_core/salsa20/ref/core_salsa20.c
  src/libsodium/crypto_core/hsalsa20/ref2/core_hsalsa20.c
  src/libsodium/crypto_core/hsalsa20/core_hsalsa20_api.c
  src/libsodium/crypto_core/salsa208/core_salsa208_api.c
  src/libsodium/crypto_core/salsa208/ref/core_salsa208.c
  src/libsodium/crypto_hashblocks/sha512/ref/blocks_sha512.c
  src/libsodium/crypto_hashblocks/sha512/hashblocks_sha512_api.c
  src/libsodium/crypto_hashblocks/sha256/ref/blocks_sha256.c
  src/libsodium/crypto_hashblocks/sha256/hashblocks_sha256_api.c
  src/libsodium/crypto_verify/16/verify_16_api.c
  src/libsodium/crypto_verify/16/ref/verify_16.c
  src/libsodium/crypto_verify/32/ref/verify_32.c
  src/libsodium/crypto_verify/32/verify_32_api.c
  )

add_library(crypto::sodium ALIAS sodium)

target_include_directories(sodium PUBLIC
#  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include/sodium>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include>"
  )

include_directories(
  src/libsodium/include/sodium
  ${CMAKE_CURRENT_BINARY_DIR}/sodium
)

set(tests_list
  test/default/auth2
  test/default/auth3
  test/default/auth5
  test/default/auth
  test/default/box2
  test/default/box7
  test/default/box8
  test/default/box
  test/default/box_easy2
  test/default/box_easy
  test/default/core1
  test/default/core2
  test/default/core3
  test/default/core4
  test/default/core5
  test/default/core6
  test/default/generichash2
  test/default/generichash3
  test/default/generichash
  test/default/hash3
  test/default/hash
  test/default/onetimeauth2
  test/default/onetimeauth7
  test/default/onetimeauth
  test/default/randombytes
  test/default/scalarmult2
  test/default/scalarmult5
  test/default/scalarmult6
  test/default/scalarmult7
  test/default/scalarmult8
  test/default/scalarmult
  test/default/secretbox2
  test/default/secretbox7
  test/default/secretbox8
  test/default/secretbox
  test/default/secretbox_easy2
  test/default/secretbox_easy
  test/default/shorthash
  test/default/sodium_core
  test/default/sodium_utils
  test/default/sodium_version
  test/default/stream2
  test/default/stream3
  test/default/stream4
  test/default/stream5
  test/default/stream6
  test/default/stream
  )

foreach(test IN LISTS tests_list)
  get_filename_component(basename ${test} NAME)
  get_filename_component(dirname  ${test} PATH)
  set(target "${PROJECT_NAME}-test-${basename}")
  add_executable(${target} "${test}.c")
  include_directories(test/quirks ${CMAKE_CURRENT_BINARY_DIR})
  add_definitions(-DTEST_SRCDIR=\"${dirname}\")
  target_link_libraries(${target} PRIVATE
	crypto::sodium
    )
  add_test(NAME ${PROJECT_NAME}.${test}
    COMMAND $<TARGET_FILE:${target}>
    )
endforeach()
