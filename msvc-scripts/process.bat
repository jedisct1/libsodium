cscript msvc-scripts/rep.vbs //Nologo s/@HAVE_TI_MODE_V@/0/ < src\libsodium\include\sodium\crypto_scalarmult_curve25519.h.in > src\libsodium\include\sodium\crypto_scalarmult_curve25519.h

cscript msvc-scripts/rep.vbs //Nologo s/@HAVE_AMD64_ASM_V@/0/ < src\libsodium\include\sodium\crypto_stream_salsa20.h.in > src\libsodium\include\sodium\crypto_stream_salsa20.h

cscript msvc-scripts/rep.vbs //Nologo s/@VERSION@/0.4.5/ < src\libsodium\include\sodium\version.h.in > tmp
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MAJOR@/4/ < tmp > tmp2
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MINOR@/4/ < tmp2 > src\libsodium\include\sodium\version.h
del tmp tmp2
