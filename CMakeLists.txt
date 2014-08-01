cmake_minimum_required(VERSION 2.8.4)

project("libsodium")

include (TestBigEndian)
test_big_endian(TEST_ENDIAN)

set(SODIUM_LIBRARY_VERSION_MAJOR "6")
set(SODIUM_LIBRARY_VERSION_MINOR "1")
set(SODIUM_LIBRARY_VERSION_PATCH "0")
set(VERSION "${SODIUM_LIBRARY_VERSION_MAJOR}.${SODIUM_LIBRARY_VERSION_MINOR}.${SODIUM_LIBRARY_VERSION_PATCH}")

set(CPACK_PACKAGE_NAME "libsodium")
set(CPACK_PACKAGE_VERSION_MAJOR "${SODIUM_LIBRARY_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SODIUM_LIBRARY_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SODIUM_LIBRARY_VERSION_PATCH}")
#set(CPACK_PACKAGE_DIRECTORY "cpack_dir")
set(CPACK_PACKAGE_VENDOR "mynamehere")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "mynamehere")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_TGZ "ON")
#SET(CPACK_INSTALL_CMAKE_PROJECTS "ALL")

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_LIBSODIUM_TESTS "Build libsodium tests" ON)
option(BUILD_LIBSODIUM_OUTPUT_SIMPLE "Simple output paths (outputs placed in bin/lib)" OFF)

if(BUILD_LIBSODIUM_OUTPUT_SIMPLE)
	if(MSVC)
		foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
			string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
			set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
			set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
		endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
	endif()

	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

	set(test_output_dir "${CMAKE_BINARY_DIR}/bin")
else()
endif()

list(APPEND src
	"src/libsodium/crypto_aead/chacha20poly1305/sodium/aead_chacha20poly1305.c"
	"src/libsodium/crypto_auth/crypto_auth.c"
	"src/libsodium/crypto_auth/hmacsha256/auth_hmacsha256_api.c"
	"src/libsodium/crypto_auth/hmacsha256/cp/api.h"
	"src/libsodium/crypto_auth/hmacsha256/cp/hmac_hmacsha256.c"
	"src/libsodium/crypto_auth/hmacsha256/cp/verify_hmacsha256.c"
	"src/libsodium/crypto_auth/hmacsha512/auth_hmacsha512_api.c"
	"src/libsodium/crypto_auth/hmacsha512/cp/api.h"
	"src/libsodium/crypto_auth/hmacsha512/cp/hmac_hmacsha512.c"
	"src/libsodium/crypto_auth/hmacsha512/cp/verify_hmacsha512.c"
	"src/libsodium/crypto_auth/hmacsha512256/auth_hmacsha512256_api.c"
	"src/libsodium/crypto_auth/hmacsha512256/cp/api.h"
	"src/libsodium/crypto_auth/hmacsha512256/cp/hmac_hmacsha512256.c"
	"src/libsodium/crypto_auth/hmacsha512256/cp/verify_hmacsha512256.c"
	"src/libsodium/crypto_box/crypto_box.c"
	"src/libsodium/crypto_box/crypto_box_easy.c"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/box_curve25519xsalsa20poly1305_api.c"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/api.h"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/after_curve25519xsalsa20poly1305.c"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/before_curve25519xsalsa20poly1305.c"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/box_curve25519xsalsa20poly1305.c"
	"src/libsodium/crypto_box/curve25519xsalsa20poly1305/ref/keypair_curve25519xsalsa20poly1305.c"
	"src/libsodium/crypto_core/hsalsa20/ref2/core_hsalsa20.c"
	"src/libsodium/crypto_core/hsalsa20/core_hsalsa20_api.c"
	"src/libsodium/crypto_core/hsalsa20/ref2/api.h"
	"src/libsodium/crypto_core/salsa20/ref/core_salsa20.c"
	"src/libsodium/crypto_core/salsa20/core_salsa20_api.c"
	"src/libsodium/crypto_core/salsa20/ref/api.h"
	"src/libsodium/crypto_generichash/crypto_generichash.c"
	"src/libsodium/crypto_generichash/blake2/generichash_blake2_api.c"
	"src/libsodium/crypto_generichash/blake2/ref/api.h"
	"src/libsodium/crypto_generichash/blake2/ref/blake2-impl.h"
	"src/libsodium/crypto_generichash/blake2/ref/blake2.h"
	"src/libsodium/crypto_generichash/blake2/ref/blake2b-ref.c"
	"src/libsodium/crypto_generichash/blake2/ref/generichash_blake2b.c"
	"src/libsodium/crypto_hash/crypto_hash.c"
	"src/libsodium/crypto_hash/sha256/hash_sha256_api.c"
	"src/libsodium/crypto_hash/sha256/cp/api.h"
	"src/libsodium/crypto_hash/sha256/cp/hash_sha256.c"
	"src/libsodium/crypto_hash/sha512/hash_sha512_api.c"
	"src/libsodium/crypto_hash/sha512/cp/api.h"
	"src/libsodium/crypto_hash/sha512/cp/hash_sha512.c"
	"src/libsodium/crypto_onetimeauth/crypto_onetimeauth.c"
	"src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305.c"
	"src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_api.c"
	"src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_try.c"
	"src/libsodium/crypto_onetimeauth/poly1305/donna/poly1305_donna.h"
	"src/libsodium/crypto_onetimeauth/poly1305/donna/poly1305_donna32.h"
	"src/libsodium/crypto_onetimeauth/poly1305/donna/poly1305_donna64.h"
	"src/libsodium/crypto_onetimeauth/poly1305/donna/auth_poly1305_donna.c"
	"src/libsodium/crypto_onetimeauth/poly1305/donna/verify_poly1305_donna.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/crypto_scrypt-common.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/crypto_scrypt.h"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/scrypt_platform.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.h"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/sysendian.h"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/nosse/pwhash_scryptsalsa208sha256_nosse.c"
	"src/libsodium/crypto_pwhash/scryptsalsa208sha256/sse/pwhash_scryptsalsa208sha256_sse.c"
	"src/libsodium/crypto_scalarmult/crypto_scalarmult.c"
	"src/libsodium/crypto_scalarmult/curve25519/scalarmult_curve25519_api.c"
	"src/libsodium/crypto_secretbox/crypto_secretbox.c"
	"src/libsodium/crypto_secretbox/crypto_secretbox_easy.c"
	"src/libsodium/crypto_secretbox/xsalsa20poly1305/secretbox_xsalsa20poly1305_api.c"
	"src/libsodium/crypto_secretbox/xsalsa20poly1305/ref/api.h"
	"src/libsodium/crypto_secretbox/xsalsa20poly1305/ref/box_xsalsa20poly1305.c"
	"src/libsodium/crypto_shorthash/crypto_shorthash.c"
	"src/libsodium/crypto_shorthash/siphash24/shorthash_siphash24_api.c"
	"src/libsodium/crypto_shorthash/siphash24/ref/api.h"
	"src/libsodium/crypto_shorthash/siphash24/ref/shorthash_siphash24.c"
	"src/libsodium/crypto_sign/crypto_sign.c"
	"src/libsodium/crypto_sign/ed25519/ref10/base.h"
	"src/libsodium/crypto_sign/ed25519/ref10/base2.h"
	"src/libsodium/crypto_sign/ed25519/sign_ed25519_api.c"
	"src/libsodium/crypto_sign/ed25519/ref10/api.h"
	"src/libsodium/crypto_sign/ed25519/ref10/d.h"
	"src/libsodium/crypto_sign/ed25519/ref10/d2.h"
	"src/libsodium/crypto_sign/ed25519/ref10/fe.h"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_0.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_1.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_add.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_cmov.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_copy.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_frombytes.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_invert.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_isnegative.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_isnonzero.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_mul.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_neg.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_pow22523.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_sq.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_sq2.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_sub.c"
	"src/libsodium/crypto_sign/ed25519/ref10/fe_tobytes.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_add.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_add.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_double_scalarmult.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_frombytes.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_madd.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_madd.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_msub.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_msub.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p1p1_to_p2.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p1p1_to_p3.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p2_0.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p2_dbl.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p2_dbl.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p3_0.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p3_dbl.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p3_to_cached.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p3_to_p2.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_p3_tobytes.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_precomp_0.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_scalarmult_base.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_sub.c"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_sub.h"
	"src/libsodium/crypto_sign/ed25519/ref10/ge_tobytes.c"
	"src/libsodium/crypto_sign/ed25519/ref10/keypair.c"
	"src/libsodium/crypto_sign/ed25519/ref10/open.c"
	"src/libsodium/crypto_sign/ed25519/ref10/pow22523.h"
	"src/libsodium/crypto_sign/ed25519/ref10/pow225521.h"
	"src/libsodium/crypto_sign/ed25519/ref10/sc.h"
	"src/libsodium/crypto_sign/ed25519/ref10/sc_muladd.c"
	"src/libsodium/crypto_sign/ed25519/ref10/sc_reduce.c"
	"src/libsodium/crypto_sign/ed25519/ref10/sign.c"
	"src/libsodium/crypto_sign/ed25519/ref10/sqrtm1.h"
	"src/libsodium/crypto_stream/crypto_stream.c"
	"src/libsodium/crypto_stream/chacha20/stream_chacha20_api.c"
	"src/libsodium/crypto_stream/chacha20/ref/api.h"
	"src/libsodium/crypto_stream/chacha20/ref/stream_chacha20_ref.c"
	"src/libsodium/crypto_stream/salsa20/stream_salsa20_api.c"
	"src/libsodium/crypto_stream/xsalsa20/stream_xsalsa20_api.c"
	"src/libsodium/crypto_stream/xsalsa20/ref/api.h"
	"src/libsodium/crypto_stream/xsalsa20/ref/stream_xsalsa20.c"
	"src/libsodium/crypto_stream/xsalsa20/ref/xor_xsalsa20.c"
	"src/libsodium/crypto_verify/16/verify_16_api.c"
	"src/libsodium/crypto_verify/16/ref/api.h"
	"src/libsodium/crypto_verify/16/ref/verify_16.c"
	"src/libsodium/crypto_verify/32/verify_32_api.c"
	"src/libsodium/crypto_verify/32/ref/api.h"
	"src/libsodium/crypto_verify/32/ref/verify_32.c"
	"src/libsodium/crypto_verify/64/verify_64_api.c"
	"src/libsodium/crypto_verify/64/ref/api.h"
	"src/libsodium/crypto_verify/64/ref/verify_64.c"
	"src/libsodium/randombytes/randombytes.c"
	"src/libsodium/randombytes/salsa20/randombytes_salsa20_random.c"
	"src/libsodium/randombytes/sysrandom/randombytes_sysrandom.c"
	"src/libsodium/sodium/core.c"
	"src/libsodium/sodium/runtime.c"
	"src/libsodium/sodium/utils.c"
	"src/libsodium/sodium/version.c"
	"src/libsodium/crypto_core/salsa2012/core_salsa2012_api.c"
	"src/libsodium/crypto_core/salsa2012/ref/core_salsa2012.c"
	"src/libsodium/crypto_core/salsa208/core_salsa208_api.c"
	"src/libsodium/crypto_core/salsa208/ref/core_salsa208.c"
	"src/libsodium/crypto_scalarmult/curve25519/donna_c64/base_curve25519_donna_c64.c"
	"src/libsodium/crypto_scalarmult/curve25519/donna_c64/smult_curve25519_donna_c64.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/base_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_0_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_1_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_add_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_copy_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_cswap_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_frombytes_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_invert_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_mul121666_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_mul_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_sq_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_sub_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_tobytes_curve25519_ref10.c"
	"src/libsodium/crypto_scalarmult/curve25519/ref10/scalarmult_curve25519_ref10.c"
	"src/libsodium/crypto_sign/edwards25519sha512batch/ref/fe25519_edwards25519sha512batch.c"
	"src/libsodium/crypto_sign/edwards25519sha512batch/ref/ge25519_edwards25519sha512batch.c"
	"src/libsodium/crypto_sign/edwards25519sha512batch/ref/sc25519_edwards25519sha512batch.c"
	"src/libsodium/crypto_sign/edwards25519sha512batch/ref/sign_edwards25519sha512batch.c"
	"src/libsodium/crypto_sign/edwards25519sha512batch/sign_edwards25519sha512batch_api.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/afternm_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/beforenm_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/common_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/consts_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/int128_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/stream_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/portable/xor_afternm_aes128ctr.c"
	"src/libsodium/crypto_stream/aes128ctr/stream_aes128ctr_api.c"
	"src/libsodium/crypto_stream/aes256estream/hongjun/aes256-ctr.c"
	"src/libsodium/crypto_stream/aes256estream/stream_aes256estream_api.c"
	"src/libsodium/crypto_stream/salsa2012/ref/stream_salsa2012.c"
	"src/libsodium/crypto_stream/salsa2012/ref/xor_salsa2012.c"
	"src/libsodium/crypto_stream/salsa2012/stream_salsa2012_api.c"
	"src/libsodium/crypto_stream/salsa208/ref/stream_salsa208.c"
	"src/libsodium/crypto_stream/salsa208/ref/xor_salsa208.c"
	"src/libsodium/crypto_stream/salsa208/stream_salsa208_api.c"
	"src/libsodium/crypto_stream/salsa20/ref/stream_salsa20_ref.c"
	"src/libsodium/crypto_stream/salsa20/ref/xor_salsa20_ref.c"
	"src/libsodium/sodium/compat.c"
	)

list(APPEND public_hdrs
	"src/libsodium/include/sodium.h"
	"src/libsodium/include/sodium/core.h"
	"src/libsodium/include/sodium/crypto_aead_chacha20poly1305.h"
	"src/libsodium/include/sodium/crypto_auth.h"
	"src/libsodium/include/sodium/crypto_auth_hmacsha256.h"
	"src/libsodium/include/sodium/crypto_auth_hmacsha512.h"
	"src/libsodium/include/sodium/crypto_auth_hmacsha512256.h"
	"src/libsodium/include/sodium/crypto_box.h"
	"src/libsodium/include/sodium/crypto_box_curve25519xsalsa20poly1305.h"
	"src/libsodium/include/sodium/crypto_core_hsalsa20.h"
	"src/libsodium/include/sodium/crypto_core_salsa20.h"
	"src/libsodium/include/sodium/crypto_core_salsa2012.h"
	"src/libsodium/include/sodium/crypto_core_salsa208.h"
	"src/libsodium/include/sodium/crypto_generichash.h"
	"src/libsodium/include/sodium/crypto_generichash_blake2b.h"
	"src/libsodium/include/sodium/crypto_hash.h"
	"src/libsodium/include/sodium/crypto_hash_sha256.h"
	"src/libsodium/include/sodium/crypto_hash_sha512.h"
	"src/libsodium/include/sodium/crypto_onetimeauth.h"
	"src/libsodium/include/sodium/crypto_onetimeauth_poly1305.h"
	"src/libsodium/include/sodium/crypto_pwhash_scryptsalsa208sha256.h"
	"src/libsodium/include/sodium/crypto_scalarmult.h"
	"src/libsodium/include/sodium/crypto_scalarmult_curve25519.h"
	"src/libsodium/include/sodium/crypto_secretbox.h"
	"src/libsodium/include/sodium/crypto_secretbox_xsalsa20poly1305.h"
	"src/libsodium/include/sodium/crypto_shorthash.h"
	"src/libsodium/include/sodium/crypto_shorthash_siphash24.h"
	"src/libsodium/include/sodium/crypto_sign.h"
	"src/libsodium/include/sodium/crypto_sign_ed25519.h"
	"src/libsodium/include/sodium/crypto_stream.h"
	"src/libsodium/include/sodium/crypto_stream_aes128ctr.h"
	"src/libsodium/include/sodium/crypto_stream_aes256estream.h"
	"src/libsodium/include/sodium/crypto_stream_chacha20.h"
	"src/libsodium/include/sodium/crypto_stream_salsa20.h"
	"src/libsodium/include/sodium/crypto_stream_salsa2012.h"
	"src/libsodium/include/sodium/crypto_stream_salsa208.h"
	"src/libsodium/include/sodium/crypto_stream_xsalsa20.h"
	"src/libsodium/include/sodium/crypto_verify_16.h"
	"src/libsodium/include/sodium/crypto_verify_32.h"
	"src/libsodium/include/sodium/crypto_verify_64.h"
	"src/libsodium/include/sodium/randombytes.h"
	"src/libsodium/include/sodium/randombytes_salsa20_random.h"
	"src/libsodium/include/sodium/randombytes_sysrandom.h"
	"src/libsodium/include/sodium/runtime.h"
	"src/libsodium/include/sodium/utils.h"
	"${CMAKE_BINARY_DIR}/sodium/version.h"
	)

configure_file("src/libsodium/include/sodium/version.h.in"
	"${CMAKE_BINARY_DIR}/sodium/version.h")

list(APPEND hdr
	"")

if(NOT HAVE_TI_MODE)
	list(APPEND src
		"src/libsodium/crypto_scalarmult/curve25519/ref10/api.h"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/base_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe.h"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_0_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_1_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_add_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_copy_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_cswap_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_frombytes_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_invert_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_mul_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_mul121666_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_sq_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_sub_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/fe_tobytes_curve25519_ref10.c"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/montgomery.h"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/pow225521.h"
		"src/libsodium/crypto_scalarmult/curve25519/ref10/scalarmult_curve25519_ref10.c"
		)
endif()

list(APPEND public_include_dirs
	"${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include")

list(APPEND include_dirs
	"${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include/sodium"
	"${CMAKE_BINARY_DIR}/sodium")

if(NOT TEST_ENDIAN)
	list(APPEND definitions_private
		"NATIVE_LITTLE_ENDIAN"
		)
endif()

if(WIN32)
	list(APPEND definitions_private
		"HAVE_VIRTUALLOCK")
endif()

if(MSVC)
	list(APPEND definitions_private
		"inline=__inline"
		"_CRT_SECURE_NO_WARNINGS"
		)
endif()

if(BUILD_SHARED_LIBS)
	list(APPEND definitions_private
		"DLL_EXPORT")
	set(libtype "SHARED")
else()
	list(APPEND definitions_public
		"SODIUM_STATIC")
	set(libtype "STATIC")
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
	configure_file("builds/msvc/resource.rc.in"
		"${CMAKE_BINARY_DIR}/resource.rc" @ONLY WIN32)

	configure_file("builds/msvc/resource.h"
		"${CMAKE_BINARY_DIR}/resource.h" COPYONLY @ONLY WIN32)

	# append resource to DLL
	list(APPEND src
		"${CMAKE_BINARY_DIR}/resource.rc")
endif()

add_library(libsodium ${libtype} ${src} ${hdr} ${public_hdrs})
set_target_properties(libsodium PROPERTIES
	INCLUDE_DIRECTORIES "${include_dirs}"
	COMPILE_DEFINITIONS "${definitions_public};${definitions_private}"
	INTERFACE_INCLUDE_DIRECTORIES "${public_include_dirs}"
	INTERFACE_COMPILE_DEFINITIONS "${definitions_public}"
	PREFIX ""
	SOVERSION ${SODIUM_LIBRARY_VERSION_MAJOR}
	)

install(TARGETS libsodium
	LIBRARY DESTINATION lib COMPONENT lib
	ARCHIVE DESTINATION lib COMPONENT lib
	RUNTIME DESTINATION bin COMPONENT bin)

# cannot use PUBLIC_HEADER property on libsodium target to install headers
# as we need to preserve the directory structure
install(DIRECTORY "src/libsodium/include/sodium"
	DESTINATION "include" COMPONENT dev
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.h.in" EXCLUDE)

install(FILES "src/libsodium/include/sodium.h"
	DESTINATION "include" COMPONENT dev)

install(FILES "${CMAKE_BINARY_DIR}/sodium/version.h"
	DESTINATION "include/sodium" COMPONENT dev)

### TESTS
if(BUILD_LIBSODIUM_TESTS)
	enable_testing()

	if(MSVC)
		list(APPEND test_defines
			"_CRT_SECURE_NO_WARNINGS")
	endif()

	list(APPEND test_defines
		"TEST_SRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}/test/default\"")

	list(APPEND test_inc_dirs
		"${CMAKE_CURRENT_SOURCE_DIR}/test/quirks"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include/sodium"
		"${CMAKE_BINARY_DIR}")

	macro(buildtest testname testfiles)
		set(exename "test_${testname}")
		add_executable(${exename} ${testfiles})
		set_target_properties(${exename} PROPERTIES
			INCLUDE_DIRECTORIES "${test_inc_dirs}"
			COMPILE_DEFINITIONS "${test_defines}")
		target_link_libraries(${exename} libsodium)
		add_test("${exename}" ${test_output_dir}/${exename} WORKING_DIRECTORY "test/default")
	endmacro()

	buildtest(aead_chacha20poly1305 "test/default/aead_chacha20poly1305.c")
	buildtest(auth "test/default/auth.c")
	buildtest(auth2 "test/default/auth2.c")
	buildtest(auth3 "test/default/auth3.c")
	buildtest(auth5 "test/default/auth5.c")
	buildtest(auth6 "test/default/auth6.c")
	buildtest(auth7 "test/default/auth7.c")
	buildtest(box "test/default/box.c")
	buildtest(box2 "test/default/box2.c")
	buildtest(box7 "test/default/box7.c")
	buildtest(box8 "test/default/box8.c")
	buildtest(box_easy "test/default/box_easy.c")
	buildtest(box_easy2 "test/default/box_easy2.c")
	buildtest(box_seed "test/default/box_seed.c")
	buildtest(chacha20 "test/default/chacha20.c")
	buildtest(core1 "test/default/core1.c")
	buildtest(core2 "test/default/core2.c")
	buildtest(core3 "test/default/core3.c")
	buildtest(core4 "test/default/core4.c")
	buildtest(core5 "test/default/core5.c")
	buildtest(core6 "test/default/core6.c")
	buildtest(generichash "test/default/generichash.c")
	buildtest(generichash2 "test/default/generichash2.c")
	buildtest(generichash3 "test/default/generichash3.c")
	buildtest(hash "test/default/hash.c")
	buildtest(hash3 "test/default/hash3.c")
	buildtest(onetimeauth "test/default/onetimeauth.c")
	buildtest(onetimeauth2 "test/default/onetimeauth2.c")
	buildtest(onetimeauth7 "test/default/onetimeauth7.c")
	buildtest(pwhash "test/default/pwhash.c")
	buildtest(pwhash_scrypt_ll "test/default/pwhash_scrypt_ll.c")
	buildtest(randombytes "test/default/randombytes.c")
	buildtest(scalarmult "test/default/scalarmult.c")
	buildtest(scalarmult2 "test/default/scalarmult2.c")
	buildtest(scalarmult5 "test/default/scalarmult5.c")
	buildtest(scalarmult6 "test/default/scalarmult6.c")
	buildtest(secretbox "test/default/secretbox.c")
	buildtest(secretbox2 "test/default/secretbox2.c")
	buildtest(secretbox7 "test/default/secretbox7.c")
	buildtest(secretbox8 "test/default/secretbox8.c")
	buildtest(secretbox_easy "test/default/secretbox_easy.c")
	buildtest(secretbox_easy2 "test/default/secretbox_easy2.c")
	buildtest(shorthash "test/default/shorthash.c")
	buildtest(sign "test/default/sign.c")
	buildtest(sodium_core "test/default/sodium_core.c")
	buildtest(sodium_utils "test/default/sodium_utils.c")
	buildtest(sodium_version "test/default/sodium_version.c")
	buildtest(stream "test/default/stream.c")
	buildtest(stream2 "test/default/stream2.c")
	buildtest(stream3 "test/default/stream3.c")
	buildtest(stream4 "test/default/stream4.c")
	buildtest(stream5 "test/default/stream5.c")
	buildtest(stream6 "test/default/stream6.c")
	buildtest(verify1 "test/default/verify1.c")
endif()

include(CPack)
