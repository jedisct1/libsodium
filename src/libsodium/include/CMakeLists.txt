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

SET(LIBSODIUM_SOURCE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")
SET(LIBSODIUM_BINARY_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "")

SET(_HEADERS
	sodium.h

	sodium/core.h
	sodium/crypto_auth.h
	sodium/crypto_auth_hmacsha256.h
	sodium/crypto_auth_hmacsha512256.h
	sodium/crypto_box.h
	sodium/crypto_box_curve25519xsalsa20poly1305.h
	sodium/crypto_core_hsalsa20.h
	sodium/crypto_core_salsa20.h
	sodium/crypto_core_salsa2012.h
	sodium/crypto_core_salsa208.h
	sodium/crypto_generichash.h
	sodium/crypto_generichash_blake2b.h
	sodium/crypto_hash.h
	sodium/crypto_hash_sha256.h
	sodium/crypto_hash_sha512.h
	sodium/crypto_hashblocks_sha256.h
	sodium/crypto_hashblocks_sha512.h
	sodium/crypto_onetimeauth.h
	sodium/crypto_onetimeauth_poly1305.h
	sodium/crypto_onetimeauth_poly1305_53.h
	sodium/crypto_onetimeauth_poly1305_donna.h
	sodium/crypto_scalarmult.h
	sodium/crypto_secretbox.h
	sodium/crypto_secretbox_xsalsa20poly1305.h
	sodium/crypto_shorthash.h
	sodium/crypto_shorthash_siphash24.h
	sodium/crypto_sign.h
	sodium/crypto_sign_ed25519.h
	sodium/crypto_sign_edwards25519sha512batch.h
	sodium/crypto_stream.h
	sodium/crypto_stream_aes128ctr.h
	sodium/crypto_stream_aes256estream.h
	sodium/crypto_stream_salsa2012.h
	sodium/crypto_stream_salsa208.h
	sodium/crypto_stream_xsalsa20.h
	sodium/crypto_int32.h
	sodium/crypto_int64.h
	sodium/crypto_uint16.h
	sodium/crypto_uint32.h
	sodium/crypto_uint64.h
	sodium/crypto_uint8.h
	sodium/crypto_verify_16.h
	sodium/crypto_verify_32.h
	sodium/export.h
	sodium/randombytes.h
	sodium/randombytes_salsa20_random.h
	sodium/randombytes_sysrandom.h
	sodium/utils.h
)

FOREACH(HEADER ${_HEADERS})
    STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    INSTALL(FILES ${HEADER} DESTINATION ${LIBSODIUM_INCLUDE_DIR}/${DIR})
ENDFOREACH(HEADER ${HEADERS})




SET(_HEADERS_IN
	sodium/crypto_scalarmult_curve25519.h
	sodium/crypto_stream_salsa20.h
	sodium/version.h
)

FOREACH(HEADER ${_HEADERS_IN})
    STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${HEADER} DESTINATION ${LIBSODIUM_INCLUDE_DIR}/${DIR})
ENDFOREACH(HEADER ${HEADERS})


ADD_SUBDIRECTORY(sodium)
