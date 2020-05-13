cscript msvc-scripts/rep.vbs //Nologo s/@VERSION@/1.0.18/ < src\libsodium\include\sodium\version.h.in > tmp
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MAJOR@/11/ < tmp > tmp2
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_VERSION_MINOR@/0/ < tmp2 > tmp3
cscript msvc-scripts/rep.vbs //Nologo s/@SODIUM_LIBRARY_MINIMAL_DEF@// < tmp3 > src\libsodium\include\sodium\version.h
del tmp tmp2 tmp3
