
project(sodium)
cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0028 NEW)
enable_testing()


#SODIUM_STATIC

include(CheckCSourceCompiles)
include(CheckFunctionExists)
include(TestBigEndian)

file(WRITE  ${CMAKE_BINARY_DIR}/Modules/FindSodium.cmake "set(SODIUM_FOUND 1)\n")
file(APPEND ${CMAKE_BINARY_DIR}/Modules/FindSodium.cmake "set(SODIUM_LIBRARY crypto::sodium)\n")
#set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/Modules)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/Modules)

######################################################################
CHECK_C_SOURCE_COMPILES(
  "
int main(void) {
#ifndef __GNUC__
# error mode(TI) is a gcc extension
#endif
#if defined(__clang__) && !defined(__x86_64__)
# error clang doesn't properly compile smult_curve25519_donna_c64.c
#endif
#ifndef NATIVE_LITTLE_ENDIAN
# error donna_c64 currently requires a little endian CPU
#endif
#include <stdint.h>
typedef unsigned uint128_t __attribute__((mode(TI)));
void fcontract(uint128_t *t) {
  *t += 0x8000000000000 - 1;
}
(void) fcontract;
};
    "
  HAVE_TI_MODE)


######################################################################
CHECK_C_SOURCE_COMPILES(
  "
int main(void) {
#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
/* neat */
#else
# error !amd64
#endif
__asm__(\"pxor %xmm12,%xmm6\");
};
"
  HAVE_AMD64_ASM
  )


CHECK_C_SOURCE_COMPILES(
  "
#pragma GCC target(\"mmx\")
#include <mmintrin.h>
int main(void) {
 __m64 x = _mm_setzero_si64();
};
"
  HAVE_MMINTRIN_H)

if(HAVE_MMINTRIN_H)
  ADD_DEFINITIONS("-DHAVE_MMINTRIN_H=1")
endif()


CHECK_C_SOURCE_COMPILES(
  "
#pragma GCC target(\"sse2\")
#include <emmintrin.h>
int main(void) {
 __m128d x = _mm_setzero_pd();
};
"
  HAVE_EMMINTRIN_H
  )

if(HAVE_EMINTRIN_H)
  ADD_DEFINITIONS("-DHAVE_EMINTRIN_H=1")
endif()

CHECK_C_SOURCE_COMPILES(
  "
#pragma GCC target(\"sse3\")
#include <pmmintrin.h>
int main(void) {
 __m128 x = _mm_addsub_ps(_mm_cvtpd_ps(_mm_setzero_pd()),
                                _mm_cvtpd_ps(_mm_setzero_pd()));
};
"
  HAVE_PMMINTRIN_H
  )

if(HAVE_PMINTRIN_H)
  ADD_DEFINITIONS("-DHAVE_PMINTRIN_H=1")
endif()


CHECK_C_SOURCE_COMPILES(
  "
#pragma GCC target(\"ssse3\")
#include <tmmintrin.h>
int main(void) {
 __m64 x = _mm_abs_pi32(_m_from_int(0));
};
"
  HAVE_TMMINTRIN_H
  )
if(HAVE_TMINTRIN_H)
  ADD_DEFINITIONS("-DHAVE_TMINTRIN_H=1")
endif()

CHECK_C_SOURCE_COMPILES(
  "
#include <string.h>
int main(void) {
char dummy[42];
(void) memset_s(dummy, (rsize_t) sizeof dummy, 0, (rsize_t) sizeof dummy);
};
"
  HAVE_MEMSET_S
  )

if(HAVE_MEMSET_S)
  ADD_DEFINITIONS("-DHAVE_MEMSET_S=1")
endif()

CHECK_C_SOURCE_COMPILES(
  "
int main(void) {
unsigned int cpu_info[4];
__asm__ __volatile__ (\"xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1\" :
                      \"=a\" (cpu_info[0]), \"=&r\" (cpu_info[1]),
                      \"=c\" (cpu_info[2]), \"=d\" (cpu_info[3]) :
                      \"0\" (0U), \"2\" (0U));
};
"
  HAVE_CPUID_V
  )

if(HAVE_CPUID_V)
  ADD_DEFINITIONS("-DHAVE_CPUID_V=1")
endif()


######################################################################
TEST_BIG_ENDIAN(BE)
if(${BE})
  ADD_DEFINITIONS("-DNATIVE_BIG_ENDIAN=1")
else()
  ADD_DEFINITIONS("-DNATIVE_LITTLE_ENDIAN=1")
endif()
set(CMAKE_REQUIRED_LIBRARIES rt)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
set(CMAKE_REQUIRED_LIBRARIES )

if(HAVE_CLOCK_GETTIME)
  add_definitions(-DHAVE_CLOCK_GETTIME=1)
endif()

set(CMAKE_REQUIRED_LIBRARIES m)
check_function_exists(fegetenv HAVE_FEGETENV)
set(CMAKE_REQUIRED_LIBRARIES )

if(HAVE_FEGETENV)
  add_definitions(-DHAVE_FEGETENV=1)
endif()

check_function_exists(VirtualLock HAVE_VIRTUALLOCK)
if(HAVE_VIRTUALLOCK)
  add_definitions(-DHAVE_VIRTUALLOCK=1)
endif()

check_function_exists(mlock HAVE_MLOCK)
if(HAVE_MLOCK)
  add_definitions(-DHAVE_MLOCK=1)
endif()


check_function_exists(SecureZeroMemory HAVE_SECUREZEROMEMORY)
if(HAVE_SECUREZEROMEMORY)
  add_definitions(-DHAVE_SECUREZEROMEMORY=1)
endif()

check_function_exists(explicit_bzero HAVE_EXPLICIT_BZERO)
if(HAVE_EXPLICIT_BZERO)
  add_definitions(-DHAVE_EXPLICIT_BZERO=1)
endif()

check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
if(HAVE_POSIX_MEMALIGN)
  add_definitions(-DHAVE_POSIX_MEMALIGN=1)
endif()

string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
if(SYSTEM_PROCESSOR MATCHES "i386|i686|x86|x86_64|powerpc*|s390*")
  message(STATUS "data alignment is not required on this target")
else()
  message(STATUS "data alignment is     required on this target")
  add_definitions(-DCPU_ALIGNED_ACCESS_REQUIRED)
endif()

CONFIGURE_FILE(
  src/libsodium/include/sodium/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/sodium/version.h
  )

if(HAVE_TI_MODE)
  add_definitions(-DHAVE_TI_MODE)
  set(curve25519_src
	src/libsodium/crypto_scalarmult/curve25519/donna_c64/api.h
	src/libsodium/crypto_scalarmult/curve25519/donna_c64/base_curve25519_donna_c64.c
	src/libsodium/crypto_scalarmult/curve25519/donna_c64/smult_curve25519_donna_c64.c
	)
else()
  set(curve25519_src
	src/libsodium/crypto_scalarmult/curve25519/ref/api.h
	src/libsodium/crypto_scalarmult/curve25519/ref/base_curve25519_ref.c
	src/libsodium/crypto_scalarmult/curve25519/ref/smult_curve25519_ref.c
	)
endif()


if(HAVE_AMD64_ASM)
  SET(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "s;S")
  ENABLE_LANGUAGE(ASM)
  add_definitions(-DHAVE_AMD64_ASM)
  set(amd64_asm_src
	src/libsodium/crypto_stream/salsa20/amd64_xmm6/api.h
	src/libsodium/crypto_stream/salsa20/amd64_xmm6/stream_salsa20_amd64_xmm6.S
	)
else()
  set(amd64_asm_src
	src/libsodium/crypto_stream/salsa20/ref/api.h
	src/libsodium/crypto_stream/salsa20/ref/stream_salsa20_ref.c
	src/libsodium/crypto_stream/salsa20/ref/xor_salsa20_ref.c
	)
endif()

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
  src/libsodium/crypto_hash/sha512/cp/hash_sha512.c
  src/libsodium/crypto_hash/sha512/hash_sha512_api.c
  src/libsodium/crypto_hash/sha256/cp/hash_sha256.c
  src/libsodium/crypto_hash/sha256/hash_sha256_api.c
  src/libsodium/crypto_hash/crypto_hash.c
  src/libsodium/sodium/utils.c
  src/libsodium/sodium/version.c
  src/libsodium/sodium/core.c
  src/libsodium/sodium/runtime.c
  src/libsodium/sodium/compat.c
  src/libsodium/crypto_onetimeauth/poly1305/donna/auth_poly1305_donna.c
  src/libsodium/crypto_onetimeauth/poly1305/donna/verify_poly1305_donna.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_api.c
  src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305_try.c
  src/libsodium/crypto_onetimeauth/crypto_onetimeauth.c
  src/libsodium/crypto_auth/crypto_auth.c
  src/libsodium/crypto_auth/hmacsha256/cp/hmac_hmacsha256.c
  src/libsodium/crypto_auth/hmacsha256/cp/verify_hmacsha256.c
  src/libsodium/crypto_auth/hmacsha256/auth_hmacsha256_api.c
  src/libsodium/crypto_auth/hmacsha512/cp/hmac_hmacsha512.c
  src/libsodium/crypto_auth/hmacsha512/cp/verify_hmacsha512.c
  src/libsodium/crypto_auth/hmacsha512256/cp/hmac_hmacsha512256.c
  src/libsodium/crypto_auth/hmacsha512256/cp/verify_hmacsha512256.c
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
  ${curve25519_src}
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
  ${amd64_asm_src}
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
  src/libsodium/crypto_verify/16/verify_16_api.c
  src/libsodium/crypto_verify/16/ref/verify_16.c
  src/libsodium/crypto_verify/32/ref/verify_32.c
  src/libsodium/crypto_verify/32/verify_32_api.c
  src/libsodium/crypto_verify/64/ref/verify_64.c
  src/libsodium/crypto_verify/64/verify_64_api.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/nosse/pwhash_scryptsalsa208sha256_nosse.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/crypto_scrypt-common.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/sse/pwhash_scryptsalsa208sha256_sse.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/scrypt_platform.c
  src/libsodium/crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.c
  src/libsodium/crypto_stream/chacha20/stream_chacha20_api.c
  src/libsodium/crypto_stream/chacha20/ref/stream_chacha20_ref.c
  )

add_library(crypto::sodium ALIAS sodium)

target_include_directories(sodium PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include/sodium>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  )
set_property(TARGET sodium PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(sodium PRIVATE
  src/libsodium/include/sodium
  ${CMAKE_CURRENT_BINARY_DIR}/sodium
  )

set(tests_list
  test/default/auth2
  test/default/auth3
  test/default/auth5
  test/default/auth6
  test/default/auth7
  test/default/auth
  test/default/box2
  test/default/box7
  test/default/box8
  test/default/box
  test/default/box_easy2
  test/default/box_easy
  test/default/chacha20
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
  test/default/pwhash
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
  test/default/sign
  test/default/sodium_core
  test/default/sodium_utils
  test/default/sodium_version
  test/default/stream2
  test/default/stream3
  test/default/stream4
  test/default/stream5
  test/default/stream6
  test/default/stream
  test/default/verify1
  )


set(CTEST_OUTPUT_ON_FAILURE ON)
foreach(test IN LISTS tests_list)
  get_filename_component(basename ${test} NAME)
  get_filename_component(dirname  ${test} PATH)
  get_filename_component(absdirname  ${dirname} ABSOLUTE)
  set(target "${PROJECT_NAME}-test-${basename}")
  add_executable(${target} "${test}.c")
  target_include_directories(
	${target} PRIVATE
	test/quirks
	${CMAKE_CURRENT_BINARY_DIR}
	src/libsodium/include/sodium
	)
  add_definitions(-DTEST_SRCDIR=\"${absdirname}\")
  target_link_libraries(${target} PRIVATE
	crypto::sodium
    )
  add_test(NAME ${PROJECT_NAME}.${test}
    COMMAND $<TARGET_FILE:${target}>
    )
endforeach()
