# Generate version.h header first
subdir('include')

sources = [
	'crypto_aead/chacha20poly1305/sodium/aead_chacha20poly1305.c',
	'crypto_aead/xchacha20poly1305/sodium/aead_xchacha20poly1305.c',
	'crypto_auth/crypto_auth.c',
	'crypto_auth/hmacsha256/auth_hmacsha256.c',
	'crypto_auth/hmacsha512/auth_hmacsha512.c',
	'crypto_auth/hmacsha512256/auth_hmacsha512256.c',
	'crypto_box/crypto_box.c',
	'crypto_box/crypto_box_easy.c',
	'crypto_box/crypto_box_seal.c',
	'crypto_box/curve25519xsalsa20poly1305/box_curve25519xsalsa20poly1305.c',
	'crypto_core/curve25519/ref10/base.h',
	'crypto_core/curve25519/ref10/base2.h',
	'crypto_core/curve25519/ref10/curve25519_ref10.c',
	'crypto_core/hchacha20/core_hchacha20.c',
	'crypto_core/hsalsa20/ref2/core_hsalsa20_ref2.c',
	'crypto_core/hsalsa20/core_hsalsa20.c',
	'crypto_core/salsa/ref/core_salsa_ref.c',
	'crypto_generichash/crypto_generichash.c',
	'crypto_generichash/blake2b/generichash_blake2.c',
	'crypto_generichash/blake2b/ref/blake2.h',
	'crypto_generichash/blake2b/ref/blake2b-compress-ref.c',
	'crypto_generichash/blake2b/ref/blake2b-load-sse2.h',
	'crypto_generichash/blake2b/ref/blake2b-load-sse41.h',
	'crypto_generichash/blake2b/ref/blake2b-load-avx2.h',
	'crypto_generichash/blake2b/ref/blake2b-ref.c',
	'crypto_generichash/blake2b/ref/generichash_blake2b.c',
	'crypto_hash/crypto_hash.c',
	'crypto_hash/sha256/hash_sha256.c',
	'crypto_hash/sha256/cp/hash_sha256_cp.c',
	'crypto_hash/sha512/hash_sha512.c',
	'crypto_hash/sha512/cp/hash_sha512_cp.c',
	'crypto_kdf/blake2b/kdf_blake2b.c',
	'crypto_kdf/crypto_kdf.c',
	'crypto_kx/crypto_kx.c',
	'crypto_onetimeauth/crypto_onetimeauth.c',
	'crypto_onetimeauth/poly1305/onetimeauth_poly1305.c',
	'crypto_onetimeauth/poly1305/onetimeauth_poly1305.h',
	'crypto_onetimeauth/poly1305/donna/poly1305_donna.h',
	'crypto_onetimeauth/poly1305/donna/poly1305_donna32.h',
	'crypto_onetimeauth/poly1305/donna/poly1305_donna64.h',
	'crypto_onetimeauth/poly1305/donna/poly1305_donna.c',
	'crypto_pwhash/argon2/argon2-core.c',
	'crypto_pwhash/argon2/argon2-core.h',
	'crypto_pwhash/argon2/argon2-encoding.c',
	'crypto_pwhash/argon2/argon2-encoding.h',
	'crypto_pwhash/argon2/argon2-fill-block-ref.c',
	'crypto_pwhash/argon2/argon2.c',
	'crypto_pwhash/argon2/argon2.h',
	'crypto_pwhash/argon2/blake2b-long.c',
	'crypto_pwhash/argon2/blake2b-long.h',
	'crypto_pwhash/argon2/blamka-round-ref.h',
	'crypto_pwhash/argon2/pwhash_argon2i.c',
	'crypto_pwhash/argon2/pwhash_argon2id.c',
	'crypto_pwhash/crypto_pwhash.c',
	'crypto_scalarmult/crypto_scalarmult.c',
	'crypto_scalarmult/curve25519/scalarmult_curve25519.c',
	'crypto_scalarmult/curve25519/scalarmult_curve25519.h',
	'crypto_secretbox/crypto_secretbox.c',
	'crypto_secretbox/crypto_secretbox_easy.c',
	'crypto_secretbox/xsalsa20poly1305/secretbox_xsalsa20poly1305.c',
	'crypto_secretstream/xchacha20poly1305/secretstream_xchacha20poly1305.c',
	'crypto_shorthash/crypto_shorthash.c',
	'crypto_shorthash/siphash24/shorthash_siphash24.c',
	'crypto_shorthash/siphash24/ref/shorthash_siphash24_ref.c',
	'crypto_shorthash/siphash24/ref/shorthash_siphash_ref.h',
	'crypto_sign/crypto_sign.c',
	'crypto_sign/ed25519/sign_ed25519.c',
	'crypto_sign/ed25519/ref10/ed25519_ref10.h',
	'crypto_sign/ed25519/ref10/keypair.c',
	'crypto_sign/ed25519/ref10/open.c',
	'crypto_sign/ed25519/ref10/sign.c',
	'crypto_stream/chacha20/stream_chacha20.c',
	'crypto_stream/chacha20/stream_chacha20.h',
	'crypto_stream/chacha20/ref/chacha20_ref.h',
	'crypto_stream/chacha20/ref/chacha20_ref.c',
	'crypto_stream/crypto_stream.c',
	'crypto_stream/salsa20/stream_salsa20.c',
	'crypto_stream/salsa20/stream_salsa20.h',
	'crypto_stream/xsalsa20/stream_xsalsa20.c',
	'crypto_verify/sodium/verify.c',
	'include/sodium/private/common.h',
	'include/sodium/private/curve25519_ref10.h',
	'include/sodium/private/implementations.h',
	'include/sodium/private/mutex.h',
	'include/sodium/private/sse2_64_32.h',
	'randombytes/randombytes.c',
	'sodium/codecs.c',
	'sodium/core.c',
	'sodium/runtime.c',
	'sodium/utils.c',
	'sodium/version.c',
]

if not is_emscripten
	sources += 'randombytes/salsa20/randombytes_salsa20_random.c'

	if is_nativeclient
		sources += 'randombytes/nativeclient/randombytes_nativeclient.c'
	else
		sources += 'randombytes/sysrandom/randombytes_sysrandom.c'
	endif
endif

if can_use_128_bits
	sources += [
		'crypto_scalarmult/curve25519/donna_c64/curve25519_donna_c64.c',
		'crypto_scalarmult/curve25519/donna_c64/curve25519_donna_c64.h',
	]
else
	sources += [
		'crypto_scalarmult/curve25519/ref10/x25519_ref10.c',
		'crypto_scalarmult/curve25519/ref10/x25519_ref10.h',
	]
endif

if can_use_x86_64_asm
	sources += [
		'crypto_stream/salsa20/xmm6/salsa20_xmm6-asm.S',
		'crypto_stream/salsa20/xmm6/salsa20_xmm6.c',
		'crypto_stream/salsa20/xmm6/salsa20_xmm6.h',
	]
else
	sources += [
		'crypto_stream/salsa20/ref/salsa20_ref.c',
		'crypto_stream/salsa20/ref/salsa20_ref.h',
	]
endif

if can_use_avx
	sources += [
		'crypto_scalarmult/curve25519/sandy2x/consts_namespace.h',
		'crypto_scalarmult/curve25519/sandy2x/curve25519_sandy2x.c',
		'crypto_scalarmult/curve25519/sandy2x/curve25519_sandy2x.h',
		'crypto_scalarmult/curve25519/sandy2x/fe.h',
		'crypto_scalarmult/curve25519/sandy2x/fe51.h',
		'crypto_scalarmult/curve25519/sandy2x/fe51_invert.c',
		'crypto_scalarmult/curve25519/sandy2x/fe51_namespace.h',
		'crypto_scalarmult/curve25519/sandy2x/fe_frombytes_sandy2x.c',
		'crypto_scalarmult/curve25519/sandy2x/ladder.h',
		'crypto_scalarmult/curve25519/sandy2x/ladder_base.h',
		'crypto_scalarmult/curve25519/sandy2x/ladder_base_namespace.h',
		'crypto_scalarmult/curve25519/sandy2x/ladder_namespace.h',
		'crypto_scalarmult/curve25519/sandy2x/sandy2x.S',
	]
endif

if not get_option('minimal')
	sources += [
		'crypto_box/curve25519xchacha20poly1305/box_curve25519xchacha20poly1305.c',
		'crypto_box/curve25519xchacha20poly1305/box_seal_curve25519xchacha20poly1305.c',
		'crypto_pwhash/scryptsalsa208sha256/crypto_scrypt-common.c',
		'crypto_pwhash/scryptsalsa208sha256/crypto_scrypt.h',
		'crypto_pwhash/scryptsalsa208sha256/scrypt_platform.c',
		'crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.c',
		'crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.h',
		'crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.c',
		'crypto_pwhash/scryptsalsa208sha256/nosse/pwhash_scryptsalsa208sha256_nosse.c',
		'crypto_secretbox/xchacha20poly1305/secretbox_xchacha20poly1305.c',
		'crypto_shorthash/siphash24/shorthash_siphashx24.c',
		'crypto_shorthash/siphash24/ref/shorthash_siphashx24_ref.c',
		'crypto_sign/ed25519/ref10/obsolete.c',
		'crypto_stream/aes128ctr/nacl/afternm_aes128ctr.c',
		'crypto_stream/aes128ctr/nacl/beforenm_aes128ctr.c',
		'crypto_stream/aes128ctr/nacl/common.h',
		'crypto_stream/aes128ctr/nacl/consts.h',
		'crypto_stream/aes128ctr/nacl/consts_aes128ctr.c',
		'crypto_stream/aes128ctr/nacl/int128.h',
		'crypto_stream/aes128ctr/nacl/int128_aes128ctr.c',
		'crypto_stream/aes128ctr/nacl/stream_aes128ctr_nacl.c',
		'crypto_stream/aes128ctr/nacl/xor_afternm_aes128ctr.c',
		'crypto_stream/aes128ctr/stream_aes128ctr.c',
		'crypto_stream/salsa2012/ref/stream_salsa2012_ref.c',
		'crypto_stream/salsa2012/stream_salsa2012.c',
		'crypto_stream/salsa208/ref/stream_salsa208_ref.c',
		'crypto_stream/salsa208/stream_salsa208.c',
		'crypto_stream/xchacha20/stream_xchacha20.c'
	]
endif

include_dirs = [
	include_directories('include/sodium'),
	version_dir,
]
dependencies = [threads]

# libaesni
libaesni_sources = [
	'crypto_aead/aes256gcm/aesni/aead_aes256gcm_aesni.c',
]
libaesni = static_library(
	'aesni', libaesni_sources,
	dependencies : dependencies + [sse2, ssse3, aes, pclmul],
	include_directories : include_dirs,
)

# libsse2
libsse2_sources = [
	'crypto_onetimeauth/poly1305/sse2/poly1305_sse2.c',
	'crypto_onetimeauth/poly1305/sse2/poly1305_sse2.h',
]

if not get_option('minimal')
	libsse2_sources += 'crypto_pwhash/scryptsalsa208sha256/sse/pwhash_scryptsalsa208sha256_sse.c'
endif

if not can_use_x86_64_asm
	libsse2_sources += [
		'crypto_stream/salsa20/xmm6int/salsa20_xmm6int-sse2.c',
		'crypto_stream/salsa20/xmm6int/salsa20_xmm6int-sse2.h',
		'crypto_stream/salsa20/xmm6int/u0.h',
		'crypto_stream/salsa20/xmm6int/u1.h',
		'crypto_stream/salsa20/xmm6int/u4.h',
	]
endif

libsse2 = static_library(
	'sse2', libsse2_sources,
	dependencies : dependencies + [sse2],
	include_directories : include_dirs,
)

# libssse3
libssse3_sources = [
	'crypto_generichash/blake2b/ref/blake2b-compress-ssse3.c',
	'crypto_generichash/blake2b/ref/blake2b-compress-ssse3.h',
	'crypto_pwhash/argon2/argon2-fill-block-ssse3.c',
	'crypto_pwhash/argon2/blamka-round-ssse3.h',
	'crypto_stream/chacha20/dolbeau/chacha20_dolbeau-ssse3.c',
	'crypto_stream/chacha20/dolbeau/chacha20_dolbeau-ssse3.h',
	'crypto_stream/chacha20/dolbeau/u0.h',
	'crypto_stream/chacha20/dolbeau/u1.h',
	'crypto_stream/chacha20/dolbeau/u4.h',
]
libssse3 = static_library(
	'ssse3', libssse3_sources,
	dependencies : dependencies + [sse2, ssse3],
	include_directories : include_dirs,
)

# libsse41
libsse41_sources = [
	'crypto_generichash/blake2b/ref/blake2b-compress-sse41.c',
	'crypto_generichash/blake2b/ref/blake2b-compress-sse41.h',
]
libsse41 = static_library(
	'sse41', libsse41_sources,
	dependencies : dependencies + [sse2, ssse3, sse4_1],
	include_directories : include_dirs,
)

# libavx2
libavx2_sources = [
	'crypto_generichash/blake2b/ref/blake2b-compress-avx2.c',
	'crypto_generichash/blake2b/ref/blake2b-compress-avx2.h',
	'crypto_pwhash/argon2/argon2-fill-block-avx2.c',
	'crypto_pwhash/argon2/blamka-round-avx2.h',
	'crypto_stream/chacha20/dolbeau/chacha20_dolbeau-avx2.c',
	'crypto_stream/chacha20/dolbeau/chacha20_dolbeau-avx2.h',
	'crypto_stream/chacha20/dolbeau/u8.h',
	'crypto_stream/salsa20/xmm6int/salsa20_xmm6int-avx2.c',
	'crypto_stream/salsa20/xmm6int/salsa20_xmm6int-avx2.h',
	'crypto_stream/salsa20/xmm6int/u0.h',
	'crypto_stream/salsa20/xmm6int/u1.h',
	'crypto_stream/salsa20/xmm6int/u4.h',
	'crypto_stream/salsa20/xmm6int/u8.h',
]
libavx2 = static_library(
	'avx2', libavx2_sources,
	dependencies : dependencies + [sse2, ssse3, sse4_1, avx, avx2],
	include_directories : include_dirs,
)

# libsodium
link_libs = [libaesni, libsse2, libssse3, libsse41, libavx2]
if get_option('soname-versions')
	libsodium = library(
		'sodium', sources,
		include_directories : include_dirs,
		dependencies : dependencies,
		version : sodium_library_version,
		soversion : dll_version,
		link_with: link_libs,
		install : true,
	)
else
	libsodium = library(
		'sodium', sources,
		include_directories : include_dirs,
		dependencies : dependencies,
		link_with: link_libs,
		install : true,
	)
endif

# Generate libsodium.pc
pkg = import('pkgconfig')

pkg.generate(
	filebase : 'libsodium',
	name : meson.project_name(),
	description : 'A modern and easy-to-use crypto library',
	version : meson.project_version(),
	libraries : libsodium,
)
