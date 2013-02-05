
lib_LTLIBRARIES = \
	libsodium.la

libsodium_la_SOURCES = \
	crypto_core/hsalsa20/ref2/core_hsalsa20.c \
	crypto_core/hsalsa20/ref2/crypto_core.h \
	crypto_core/salsa20/ref/core_salsa20.c \
	crypto_core/salsa20/ref/crypto_core.h \
	crypto_core/salsa2012/ref/core_salsa2012.c \
	crypto_core/salsa2012/ref/crypto_core.h \
	crypto_core/salsa208/ref/core_salsa208.c \
	crypto_core/salsa208/ref/crypto_core.h \
	crypto_scalarmult/curve25519/ref/base.c \
	crypto_scalarmult/curve25519/ref/crypto_scalarmult.h \
	crypto_scalarmult/curve25519/ref/smult.c \
	crypto_hashblocks/sha256/ref/blocks_sha256.c \
	crypto_hashblocks/sha256/ref/crypto_hashblocks.h \
	crypto_hashblocks/sha512/ref/blocks_sha512.c \
	crypto_hashblocks/sha512/ref/crypto_hashblocks.h \
	crypto_hash/sha256/ref/crypto_hash.h \
	crypto_hash/sha256/ref/hash_sha256.c \
	crypto_hash/sha512/ref/crypto_hash.h \
	crypto_hash/sha512/ref/hash_sha512.c \
	crypto_verify/16/ref/crypto_verify.h \
	crypto_verify/16/ref/verify_16.c \
	crypto_verify/32/ref/crypto_verify.h \
	crypto_verify/32/ref/verify_32.c \
	crypto_auth/hmacsha256/ref/crypto_auth.h \
	crypto_auth/hmacsha256/ref/hmac_hmacsha256.c \
	crypto_auth/hmacsha256/ref/verify_hmacsha256.c \
	crypto_auth/hmacsha512256/ref/api.h \
	crypto_auth/hmacsha512256/ref/crypto_auth.h \
	crypto_auth/hmacsha512256/ref/hmac_hmacsha512256.c \
	crypto_auth/hmacsha512256/ref/verify_hmacsha512256.c \
	crypto_stream/salsa20/ref/crypto_stream.h \
	crypto_stream/salsa20/ref/stream_salsa20.c \
	crypto_stream/salsa20/ref/xor_salsa20.c \
	crypto_stream/salsa2012/ref/crypto_stream.h \
	crypto_stream/salsa2012/ref/stream_salsa2012.c \
	crypto_stream/salsa2012/ref/xor_salsa2012.c \
	crypto_stream/salsa208/ref/crypto_stream.h \
	crypto_stream/salsa208/ref/stream_salsa208.c \
	crypto_stream/salsa208/ref/xor_salsa208.c \
	crypto_stream/xsalsa20/ref/crypto_stream.h \
	crypto_stream/xsalsa20/ref/stream_xsalsa20.c \
	crypto_stream/xsalsa20/ref/xor_xsalsa20.c \
	crypto_onetimeauth/poly1305/ref/auth_poly1305.c \
	crypto_onetimeauth/poly1305/ref/crypto_onetimeauth.h \
	crypto_onetimeauth/poly1305/ref/verify_poly1305.c \
	crypto_sign/edwards25519sha512batch/ref/api.h \
	crypto_sign/edwards25519sha512batch/ref/crypto_sign.h \
	crypto_sign/edwards25519sha512batch/ref/fe25519.h \
	crypto_sign/edwards25519sha512batch/ref/fe25519_edwards25519sha512batch.c \
	crypto_sign/edwards25519sha512batch/ref/ge25519.h \
	crypto_sign/edwards25519sha512batch/ref/ge25519_edwards25519sha512batch.c \
	crypto_sign/edwards25519sha512batch/ref/sc25519.h \
	crypto_sign/edwards25519sha512batch/ref/sc25519_edwards25519sha512batch.c \
	crypto_sign/edwards25519sha512batch/ref/sign_edwards25519sha512batch.c \
	crypto_secretbox/xsalsa20poly1305/ref/box_xsalsa20poly1305.c \
	crypto_secretbox/xsalsa20poly1305/ref/crypto_secretbox.h \
	crypto_box/curve25519xsalsa20poly1305/ref/after_curve25519xsalsa20poly1305.c \
	crypto_box/curve25519xsalsa20poly1305/ref/before_curve25519xsalsa20poly1305.c \
	crypto_box/curve25519xsalsa20poly1305/ref/box_curve25519xsalsa20poly1305.c \
	crypto_box/curve25519xsalsa20poly1305/ref/crypto_box.h \
	crypto_box/curve25519xsalsa20poly1305/ref/keypair_curve25519xsalsa20poly1305.c \
	crypto_sign/ed25519/ref/crypto_sign.h \
	crypto_sign/ed25519/ref/ed25519_ed25519.c \
	crypto_sign/ed25519/ref/fe25519.h \
	crypto_sign/ed25519/ref/fe25519_ed25519.c \
	crypto_sign/ed25519/ref/ge25519.h \
	crypto_sign/ed25519/ref/ge25519_ed25519.c \
	crypto_sign/ed25519/ref/sc25519.h \
	crypto_sign/ed25519/ref/sc25519_ed25519.c \
	randombytes/randombytes.c \
	randombytes/randombytes_salsa20_random.c \
	randombytes/randombytes_sysrandom.c \
	version.c

EXTRA_DIST = \
	crypto_sign/ed25519/ref/ge25519_base.data

libsodium_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	-export-dynamic \
	-no-undefined \
	$(LIBTOOL_EXTRA_FLAGS)

libsodium_la_CPPFLAGS = \
	$(LTDLINCL) \
	-I$(top_srcdir)/src/libsodium/include/sodium

SUBDIRS = \
	include
