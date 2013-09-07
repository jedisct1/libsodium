##
 # Copyright (c) 2013
 # Massimo Cetra <mcetra at gmail dot com>
 #
 # Permission to use, copy, modify, and distribute this software for any
 # purpose with or without fee is hereby granted, provided that the above
 # copyright notice and this permission notice appear in all copies.
 #
 # THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 # WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 # MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 # ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 # WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 # ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 # OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ##

SET(_SOURCES
	crypto_auth/crypto_auth.c
	crypto_auth/hmacsha256/auth_hmacsha256_api.c
	crypto_auth/hmacsha256/ref/api.h
	crypto_auth/hmacsha256/ref/hmac_hmacsha256.c
	crypto_auth/hmacsha256/ref/verify_hmacsha256.c
	crypto_auth/hmacsha512256/auth_hmacsha512256_api.c
	crypto_auth/hmacsha512256/ref/api.h
	crypto_auth/hmacsha512256/ref/hmac_hmacsha512256.c
	crypto_auth/hmacsha512256/ref/verify_hmacsha512256.c
	crypto_box/crypto_box.c
	crypto_box/curve25519xsalsa20poly1305/box_curve25519xsalsa20poly1305_api.c
	crypto_box/curve25519xsalsa20poly1305/ref/api.h
	crypto_box/curve25519xsalsa20poly1305/ref/after_curve25519xsalsa20poly1305.c
	crypto_box/curve25519xsalsa20poly1305/ref/before_curve25519xsalsa20poly1305.c
	crypto_box/curve25519xsalsa20poly1305/ref/box_curve25519xsalsa20poly1305.c
	crypto_box/curve25519xsalsa20poly1305/ref/keypair_curve25519xsalsa20poly1305.c
	crypto_core/hsalsa20/ref2/core_hsalsa20.c
	crypto_core/hsalsa20/core_hsalsa20_api.c
	crypto_core/hsalsa20/ref2/api.h
	crypto_core/salsa20/ref/core_salsa20.c
	crypto_core/salsa20/core_salsa20_api.c
	crypto_core/salsa20/ref/api.h
	crypto_core/salsa2012/ref/core_salsa2012.c
	crypto_core/salsa2012/core_salsa2012_api.c
	crypto_core/salsa2012/ref/api.h
	crypto_core/salsa208/ref/core_salsa208.c
	crypto_core/salsa208/core_salsa208_api.c
	crypto_core/salsa208/ref/api.h
	crypto_generichash/crypto_generichash.c
	crypto_generichash/blake2/generichash_blake2_api.c
	crypto_generichash/blake2/ref/api.h
	crypto_generichash/blake2/ref/blake2-impl.h
	crypto_generichash/blake2/ref/blake2.h
	crypto_generichash/blake2/ref/blake2b-ref.c
	crypto_generichash/blake2/ref/generichash_blake2b.c
	crypto_hash/crypto_hash.c
	crypto_hash/sha256/hash_sha256_api.c
	crypto_hash/sha256/ref/api.h
	crypto_hash/sha256/ref/hash_sha256.c
	crypto_hash/sha512/hash_sha512_api.c
	crypto_hash/sha512/ref/api.h
	crypto_hash/sha512/ref/hash_sha512.c
	crypto_hashblocks/sha256/ref/blocks_sha256.c
	crypto_hashblocks/sha256/hashblocks_sha256_api.c
	crypto_hashblocks/sha256/ref/api.h
	crypto_hashblocks/sha512/ref/blocks_sha512.c
	crypto_hashblocks/sha512/hashblocks_sha512_api.c
	crypto_hashblocks/sha512/ref/api.h
	crypto_onetimeauth/crypto_onetimeauth.c
	crypto_onetimeauth/poly1305/onetimeauth_poly1305.c
	crypto_onetimeauth/poly1305/onetimeauth_poly1305_api.c
	crypto_onetimeauth/poly1305/onetimeauth_poly1305_try.c
	crypto_onetimeauth/poly1305/53/api.h
	crypto_onetimeauth/poly1305/53/auth_poly1305_53.c
	crypto_onetimeauth/poly1305/53/verify_poly1305_53.c
	crypto_onetimeauth/poly1305/donna/api.h
	crypto_onetimeauth/poly1305/donna/portable-jane.h
	crypto_onetimeauth/poly1305/donna/auth_poly1305_donna.c
	crypto_onetimeauth/poly1305/donna/verify_poly1305_donna.c
	crypto_scalarmult/crypto_scalarmult.c
	crypto_secretbox/crypto_secretbox.c
	crypto_secretbox/xsalsa20poly1305/secretbox_xsalsa20poly1305_api.c
	crypto_secretbox/xsalsa20poly1305/ref/api.h
	crypto_secretbox/xsalsa20poly1305/ref/box_xsalsa20poly1305.c
	crypto_shorthash/crypto_shorthash.c
	crypto_shorthash/siphash24/shorthash_siphash24_api.c
	crypto_shorthash/siphash24/ref/api.h
	crypto_shorthash/siphash24/ref/shorthash_siphash24.c
	crypto_sign/crypto_sign.c
	crypto_sign/ed25519/ref10/base.h
	crypto_sign/ed25519/ref10/base2.h
	crypto_sign/ed25519/sign_ed25519_api.c
	crypto_sign/ed25519/ref10/api.h
	crypto_sign/ed25519/ref10/d.h
	crypto_sign/ed25519/ref10/d2.h
	crypto_sign/ed25519/ref10/fe.h
	crypto_sign/ed25519/ref10/fe_0.c
	crypto_sign/ed25519/ref10/fe_1.c
	crypto_sign/ed25519/ref10/fe_add.c
	crypto_sign/ed25519/ref10/fe_cmov.c
	crypto_sign/ed25519/ref10/fe_copy.c
	crypto_sign/ed25519/ref10/fe_frombytes.c
	crypto_sign/ed25519/ref10/fe_invert.c
	crypto_sign/ed25519/ref10/fe_isnegative.c
	crypto_sign/ed25519/ref10/fe_isnonzero.c
	crypto_sign/ed25519/ref10/fe_mul.c
	crypto_sign/ed25519/ref10/fe_neg.c
	crypto_sign/ed25519/ref10/fe_pow22523.c
	crypto_sign/ed25519/ref10/fe_sq.c
	crypto_sign/ed25519/ref10/fe_sq2.c
	crypto_sign/ed25519/ref10/fe_sub.c
	crypto_sign/ed25519/ref10/fe_tobytes.c
	crypto_sign/ed25519/ref10/ge.h
	crypto_sign/ed25519/ref10/ge_add.c
	crypto_sign/ed25519/ref10/ge_add.h
	crypto_sign/ed25519/ref10/ge_double_scalarmult.c
	crypto_sign/ed25519/ref10/ge_frombytes.c
	crypto_sign/ed25519/ref10/ge_madd.c
	crypto_sign/ed25519/ref10/ge_madd.h
	crypto_sign/ed25519/ref10/ge_msub.c
	crypto_sign/ed25519/ref10/ge_msub.h
	crypto_sign/ed25519/ref10/ge_p1p1_to_p2.c
	crypto_sign/ed25519/ref10/ge_p1p1_to_p3.c
	crypto_sign/ed25519/ref10/ge_p2_0.c
	crypto_sign/ed25519/ref10/ge_p2_dbl.c
	crypto_sign/ed25519/ref10/ge_p2_dbl.h
	crypto_sign/ed25519/ref10/ge_p3_0.c
	crypto_sign/ed25519/ref10/ge_p3_dbl.c
	crypto_sign/ed25519/ref10/ge_p3_to_cached.c
	crypto_sign/ed25519/ref10/ge_p3_to_p2.c
	crypto_sign/ed25519/ref10/ge_p3_tobytes.c
	crypto_sign/ed25519/ref10/ge_precomp_0.c
	crypto_sign/ed25519/ref10/ge_scalarmult_base.c
	crypto_sign/ed25519/ref10/ge_sub.c
	crypto_sign/ed25519/ref10/ge_sub.h
	crypto_sign/ed25519/ref10/ge_tobytes.c
	crypto_sign/ed25519/ref10/keypair.c
	crypto_sign/ed25519/ref10/open.c
	crypto_sign/ed25519/ref10/pow22523.h
	crypto_sign/ed25519/ref10/pow225521.h
	crypto_sign/ed25519/ref10/sc.h
	crypto_sign/ed25519/ref10/sc_muladd.c
	crypto_sign/ed25519/ref10/sc_reduce.c
	crypto_sign/ed25519/ref10/sign.c
	crypto_sign/ed25519/ref10/sqrtm1.h
	crypto_sign/edwards25519sha512batch/sign_edwards25519sha512batch_api.c
	crypto_sign/edwards25519sha512batch/ref/api.h
	crypto_sign/edwards25519sha512batch/ref/fe25519.h
	crypto_sign/edwards25519sha512batch/ref/fe25519_edwards25519sha512batch.c
	crypto_sign/edwards25519sha512batch/ref/ge25519.h
	crypto_sign/edwards25519sha512batch/ref/ge25519_edwards25519sha512batch.c
	crypto_sign/edwards25519sha512batch/ref/sc25519.h
	crypto_sign/edwards25519sha512batch/ref/sc25519_edwards25519sha512batch.c
	crypto_sign/edwards25519sha512batch/ref/sign_edwards25519sha512batch.c
	crypto_stream/crypto_stream.c
	crypto_stream/aes128ctr/portable/afternm_aes128ctr.c
	crypto_stream/aes128ctr/stream_aes128ctr_api.c
	crypto_stream/aes128ctr/portable/api.h
	crypto_stream/aes128ctr/portable/beforenm_aes128ctr.c
	crypto_stream/aes128ctr/portable/common.h
	crypto_stream/aes128ctr/portable/common_aes128ctr.c
	crypto_stream/aes128ctr/portable/consts.h
	crypto_stream/aes128ctr/portable/consts_aes128ctr.c
	crypto_stream/aes128ctr/portable/int128.h
	crypto_stream/aes128ctr/portable/int128_aes128ctr.c
	crypto_stream/aes128ctr/portable/stream_aes128ctr.c
	crypto_stream/aes128ctr/portable/types.h
	crypto_stream/aes128ctr/portable/xor_afternm_aes128ctr.c
	crypto_stream/aes256estream/hongjun/aes-table.h
	crypto_stream/aes256estream/hongjun/aes256-ctr.c
	crypto_stream/aes256estream/hongjun/aes256.h
	crypto_stream/aes256estream/stream_aes256estream_api.c
	crypto_stream/aes256estream/hongjun/api.h
	crypto_stream/aes256estream/hongjun/ecrypt-sync.h
	crypto_stream/salsa2012/stream_salsa2012_api.c
	crypto_stream/salsa2012/ref/api.h
	crypto_stream/salsa2012/ref/stream_salsa2012.c
	crypto_stream/salsa2012/ref/xor_salsa2012.c
	crypto_stream/salsa208/stream_salsa208_api.c
	crypto_stream/salsa208/ref/api.h
	crypto_stream/salsa208/ref/stream_salsa208.c
	crypto_stream/salsa208/ref/xor_salsa208.c
	crypto_stream/xsalsa20/stream_xsalsa20_api.c
	crypto_stream/xsalsa20/ref/api.h
	crypto_stream/xsalsa20/ref/stream_xsalsa20.c
	crypto_stream/xsalsa20/ref/xor_xsalsa20.c
	crypto_verify/16/verify_16_api.c
	crypto_verify/16/ref/api.h
	crypto_verify/16/ref/verify_16.c
	crypto_verify/32/verify_32_api.c
	crypto_verify/32/ref/api.h
	crypto_verify/32/ref/verify_32.c
	randombytes/randombytes.c
	randombytes/salsa20/randombytes_salsa20_random.c
	randombytes/sysrandom/randombytes_sysrandom.c
	sodium/compat.c
	sodium/core.c
	sodium/utils.c
	sodium/version.c
)

IF (NOT HAVE_TI_MODE)
    MESSAGE(STATUS "TI_MODE disabled")
    SET(_SOURCES
	${_SOURCES}
	crypto_scalarmult/curve25519/ref/api.h
	crypto_scalarmult/curve25519/ref/base_curve25519_ref.c
	crypto_scalarmult/curve25519/ref/smult_curve25519_ref.c
    )
ELSE (NOT HAVE_TI_MODE)
    MESSAGE(STATUS "TI_MODE enabled")
    SET(_SOURCES
	${_SOURCES}
	crypto_scalarmult/curve25519/donna_c64/api.h
	crypto_scalarmult/curve25519/donna_c64/base_curve25519_donna_c64.c
	crypto_scalarmult/curve25519/donna_c64/smult_curve25519_donna_c64.c
    )
ENDIF (NOT HAVE_TI_MODE)


IF (NOT HAVE_AMD64_ASM)
    MESSAGE(STATUS "AMD64 ASM disabled")
    SET(_SOURCES
	${_SOURCES}
	crypto_stream/salsa20/ref/api.h
	crypto_stream/salsa20/ref/stream_salsa20_ref.c
	crypto_stream/salsa20/ref/xor_salsa20_ref.c
    )
ELSE (NOT HAVE_AMD64_ASM)
    MESSAGE(STATUS "AMD64 ASM enabled")
    SET(_SOURCES
	${_SOURCES}
	crypto_stream/salsa20/amd64_xmm6/api.h
	crypto_stream/salsa20/amd64_xmm6/stream_salsa20_amd64_xmm6.S
    )
ENDIF (NOT HAVE_AMD64_ASM)


# *****************************************************************

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sodium
    ${CMAKE_CURRENT_BINARY_DIR}/include/sodium
)

# *****************************************************************

add_library (
    libsodium
	SHARED
	    ${_SOURCES}
)

SET_TARGET_PROPERTIES(
    libsodium
    PROPERTIES
	PREFIX ""
	PROJECT_LABEL "libsodium"
	INSTALL_RPATH "${LIBSODIUM_LIB_DIR}"
        LINK_FLAGS "${_LINKER_OPTIONS}"
        VERSION   "${SODIUM_LIBRARY_VERSION_MAJOR}.${SODIUM_LIBRARY_VERSION_MINOR}"
        SOVERSION "${SODIUM_LIBRARY_VERSION_MAJOR}"
)

INSTALL(
    TARGETS
	libsodium
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    LIBRARY DESTINATION ${LIBSODIUM_LIB_DIR}
    ARCHIVE DESTINATION ${LIBSODIUM_LIB_DIR}
)

# *****************************************************************

add_library (
    libsodium-static
	STATIC
	    ${_SOURCES}
)

SET_TARGET_PROPERTIES(
    libsodium-static
    PROPERTIES
	OUTPUT_NAME libsodium
)

INSTALL(
    TARGETS
	libsodium-static
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    LIBRARY DESTINATION ${LIBSODIUM_LIB_DIR}
    ARCHIVE DESTINATION ${LIBSODIUM_LIB_DIR}
)


ADD_SUBDIRECTORY(include)
