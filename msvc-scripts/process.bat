cscript msvc-scripts/rep.vbs //Nologo s/@VERSION@/0.7.0/ < src\libsodium\include\sodium\version.h.in > tmp
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MAJOR@/7/ < tmp > tmp2
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MINOR@/0/ < tmp2 > src\libsodium\include\sodium\version.h
del tmp tmp2
