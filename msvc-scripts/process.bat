cscript msvc-scripts/rep.vbs //Nologo s/@VERSION@/0.4.5/ < src\libsodium\include\sodium\version.h.in > tmp
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MAJOR@/4/ < tmp > tmp2
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MINOR@/4/ < tmp2 > src\libsodium\include\sodium\version.h
del tmp tmp2
