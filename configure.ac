AC_PREREQ([2.65])
AC_INIT([libsodium],[0.4.5],
  [https://github.com/jedisct1/libsodium/issues],
  [libsodium],
  [https://github.com/jedisct1/libsodium])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libsodium/sodium/version.c])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AM_DEP_TRACK

AC_SUBST(VERSION)
ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

SODIUM_LIBRARY_VERSION_MAJOR=4
SODIUM_LIBRARY_VERSION_MINOR=4
DLL_VERSION=4
SODIUM_LIBRARY_VERSION=9:0:5
#                      | | |
#               +------+ | +---+
#               |        |     |
#            current:revision:age
#               |        |     |
#               |        |     +- increment if interfaces have been added
#               |        |        set to zero if interfaces have been removed
#               |        |        or changed
#               |        +- increment if source code has changed
#               |           set to zero if current is incremented
#               +- increment if interfaces have been added, removed or changed
AC_SUBST(SODIUM_LIBRARY_VERSION_MAJOR)
AC_SUBST(SODIUM_LIBRARY_VERSION_MINOR)
AC_SUBST(SODIUM_LIBRARY_VERSION)
AC_SUBST(DLL_VERSION)

LX_CFLAGS=${CFLAGS-NONE}

dnl Switches

AC_ARG_ENABLE(ssp,
[AS_HELP_STRING(--disable-ssp,Do not compile with -fstack-protector)],
[
  AS_IF([test "x$enableval" = "xno"], [
    enable_ssp="no"
  ], [
    enable_ssp="yes"
  ])
],
[
  enable_ssp="yes"
])

AC_ARG_ENABLE(asm,
[AS_HELP_STRING(--disable-asm,Disable assembly implementations)],
[
  AS_IF([test "x$enableval" = "xno"], [
    enable_asm="no"
  ], [
    enable_asm="yes"
  ])
],
[
  enable_asm="yes"
])

AS_IF([test "x$EMSCRIPTEN" != "x"],[
  enable_asm="no"
  AC_MSG_WARN([compiling to javascript - asm implementations disabled])
])

AS_CASE([$host], [x86_64-*-mingw* | x86_64-*-cygwin*], [enable_asm="no"])

AC_ARG_ENABLE(pie,
[AS_HELP_STRING(--disable-pie,Do not produce position independent executables)],
 enable_pie=$enableval, enable_pie="maybe")

AS_CASE([$host_os], [mingw*], [enable_pie="no"])

AC_ARG_ENABLE(blocking-random,
[AS_HELP_STRING(--enable-blocking-random,Use /dev/random instead of /dev/urandom)],
[
  AS_IF([test "x$enableval" = "xyes"], [
    AC_DEFINE([USE_BLOCKING_RANDOM], [1], [Use blocking random])
  ])
])

AC_ARG_WITH(safecode,
[AS_HELP_STRING(--with-safecode,For maintainers only - please do not use)],
[AS_IF([test "x$withval" = "xyes"], [
    AC_ARG_VAR([SAFECODE_HOME], [set to the safecode base directory])
    : ${SAFECODE_HOME:=/opt/safecode}
    LDFLAGS="$LDFLAGS -L${SAFECODE_HOME}/lib"
    LIBS="$LIBS -lsc_dbg_rt -lpoolalloc_bitmap -lstdc++"
    CFLAGS="$CFLAGS -fmemsafety"
  ])
])

AC_ARG_ENABLE(debug,
[AS_HELP_STRING(--enable-debug,For maintainers only - please do not use)],
[
  AS_IF([test "x$enableval" = "xyes"], [
    AS_IF([test "x$LX_CFLAGS" = "xNONE"], [
      nxflags=""
      for flag in `echo $CFLAGS`; do
        AS_CASE([$flag],
          [-O*], [ ],
          [-g*], [ ],
          [*], [AS_VAR_APPEND([nxflags], [" $flag"])])
      done
      CFLAGS="$nxflags -O0 -g3"
    ])
    CPPFLAGS="$CPPFLAGS -DDEBUG=1"
  ])
])

AC_SUBST([MAINT])

dnl Checks

PKG_PROG_PKG_CONFIG([0.25])
AM_CONDITIONAL([HAVE_PKG_CONFIG], [test "x$PKG_CONFIG" != "x"])
AC_SUBST(HAVE_PKG_CONFIG)

AC_PROG_CC_C99
AM_PROG_AS
AC_USE_SYSTEM_EXTENSIONS
CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"

AX_CHECK_COMPILE_FLAG([-fvisibility=hidden],
  [CFLAGS="$CFLAGS -fvisibility=hidden"])

AX_CHECK_COMPILE_FLAG([-fPIC], [
  AX_CHECK_LINK_FLAG([-fPIC],
    [CFLAGS="$CFLAGS -fPIC"]
  )
])

AS_IF([test "$enable_pie" != "no"],[
  AX_CHECK_COMPILE_FLAG([-fPIE], [
    AX_CHECK_LINK_FLAG([-fPIE],
      [AX_CHECK_LINK_FLAG([-pie],
        [CFLAGS="$CFLAGS -fPIE"
         LDFLAGS="$LDFLAGS -pie"])
    ])
  ])
])

AX_CHECK_COMPILE_FLAG([-fwrapv], [CFLAGS="$CFLAGS -fwrapv"])
AX_CHECK_COMPILE_FLAG([-fno-strict-aliasing], [CFLAGS="$CFLAGS -fno-strict-aliasing"])
AX_CHECK_COMPILE_FLAG([-fno-strict-overflow], [CFLAGS="$CFLAGS -fno-strict-overflow"])

LIBTOOL_EXTRA_FLAGS="-version-info $SODIUM_LIBRARY_VERSION"

AS_CASE([$host_os],
  [cygwin* | mingw* | pw32* | cegcc*], [
    AX_CHECK_LINK_FLAG([-Wl,--dynamicbase], [LDFLAGS="$LDFLAGS -Wl,--dynamicbase"])
    AX_CHECK_LINK_FLAG([-Wl,--nxcompat], [LDFLAGS="$LDFLAGS -Wl,--nxcompat"])
  ])

AS_IF([test "x$enable_ssp" != "xno"],[

AS_CASE([$host_os],
  [cygwin* | mingw* | pw32* | cegcc*], [ ],
  [dragonfly*], [
    AX_CHECK_COMPILE_FLAG([-fstack-protector], [
      AX_CHECK_LINK_FLAG([-fstack-protector],
        [CFLAGS="$CFLAGS -fstack-protector"]
      )
    ])
  ],
  [*], [
    AX_CHECK_COMPILE_FLAG([-fstack-protector-all], [
      AX_CHECK_LINK_FLAG([-fstack-protector-all],
        [CFLAGS="$CFLAGS -fstack-protector-all"]
      )
    ])
  ])
])

AX_CHECK_COMPILE_FLAG([-Winit-self], [CFLAGS="$CFLAGS -Winit-self"])
AX_CHECK_COMPILE_FLAG([-Wwrite-strings], [CFLAGS="$CFLAGS -Wwrite-strings"])
AX_CHECK_COMPILE_FLAG([-Wdiv-by-zero], [CFLAGS="$CFLAGS -Wdiv-by-zero"])
AX_CHECK_COMPILE_FLAG([-Wsometimes-uninitialized], [CFLAGS="$CFLAGS -Wsometimes-uninitialized"])

AC_ARG_VAR([CWFLAGS], [define to compilation flags for generating extra warnings])

AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wall], [CWFLAGS="$CWFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wextra], [CWFLAGS="$CWFLAGS -Wextra"])

AC_MSG_CHECKING(for clang)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[
#ifndef __clang__
be sad
#endif
]])],
  [AC_MSG_RESULT(yes)
   AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wno-unknown-warning-option],
     [CWFLAGS="$CWFLAGS -Wno-unknown-warning-option"])
  ],
  [AC_MSG_RESULT(no)
])

AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wbad-function-cast], [CWFLAGS="$CWFLAGS -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcast-align], [CWFLAGS="$CWFLAGS -Wcast-align"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcast-qual], [CWFLAGS="$CWFLAGS -Wcast-qual"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wchar-subscripts], [CWFLAGS="$CWFLAGS -Wchar-subscripts"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcomment], [CWFLAGS="$CWFLAGS -Wcomment"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wfloat-equal], [CWFLAGS="$CWFLAGS -Wfloat-equal"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wformat=2], [CWFLAGS="$CWFLAGS -Wformat=2"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wimplicit], [CWFLAGS="$CWFLAGS -Wimplicit"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-declarations], [CWFLAGS="$CWFLAGS -Wmissing-declarations"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-prototypes], [CWFLAGS="$CWFLAGS -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wnormalized=id], [CWFLAGS="$CWFLAGS -Wnormalized=id"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Woverride-init], [CWFLAGS="$CWFLAGS -Woverride-init"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wparentheses], [CWFLAGS="$CWFLAGS -Wparentheses"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wpointer-arith], [CWFLAGS="$CWFLAGS -Wpointer-arith"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wredundant-decls], [CWFLAGS="$CWFLAGS -Wredundant-decls"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wstrict-prototypes], [CWFLAGS="$CWFLAGS -Wstrict-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wswitch-enum], [CWFLAGS="$CWFLAGS -Wswitch-enum"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wvariable-decl], [CWFLAGS="$CWFLAGS -Wvariable-decl"])

AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS="$LDFLAGS -Wl,-z,relro"])
AX_CHECK_LINK_FLAG([-Wl,-z,now], [LDFLAGS="$LDFLAGS -Wl,-z,now"])
AX_CHECK_LINK_FLAG([-Wl,-z,noexecstack], [LDFLAGS="$LDFLAGS -Wl,-z,noexecstack"])

LT_INIT
AC_SUBST(LIBTOOL_DEPS)

AC_ARG_VAR([AR], [path to the ar utility])
AC_CHECK_TOOL([AR], [ar], [ar])

dnl Checks for headers
AC_CHECK_HEADERS([emmintrin.h], [], [], [
#pragma GCC target("sse2")
])

AC_CHECK_HEADERS([tmmintrin.h], [], [], [
#pragma GCC target("ssse3")
])

AC_CHECK_HEADERS([smmintrin.h], [], [], [
#pragma GCC target("sse4.1")
])

AC_CHECK_HEADERS([immintrin.h], [], [], [
#pragma GCC target("avx")
])

AC_CHECK_HEADERS([avxintrin.h], [], [], [
#pragma GCC target("avx")
])

AC_CHECK_HEADERS([x86intrin.h], [], [], [
#pragma GCC target("xop")
])

AC_CHECK_HEADERS([wmmintrin.h], [], [], [
#pragma GCC target("aes")
])

AC_MSG_CHECKING(for access to floating-point rounding mode)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  #include <fenv.h>
  #pragma STDC FENV_ACCESS ON
]], [[
  const int previous_rounding_mode = fegetround();
  fesetround(FE_TONEAREST);
  fesetround(previous_rounding_mode);
]])],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_FENV_H], [1], [floating-point rounding mode is accessible])
],
[AC_MSG_RESULT(no)])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_INLINE
AC_C_BIGENDIAN(
  AC_DEFINE(NATIVE_BIG_ENDIAN, 1, [machine is bigendian]),
  AC_DEFINE(NATIVE_LITTLE_ENDIAN, 1, [machine is littleendian]),
  AC_MSG_ERROR([unknown endianess]),
  AC_MSG_WARN([universal endianess])
)

AC_MSG_CHECKING(whether __STDC_LIMIT_MACROS is required)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <limits.h>
#include <stdint.h>
]], [[
(void) SIZE_MAX;
(void) UINT64_MAX;
]])],
  [AC_MSG_RESULT(no)],
  [AC_MSG_RESULT(yes)
   CPPFLAGS="$CPPFLAGS -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS"
])

HAVE_AMD64_ASM_V=0
AS_IF([test "$enable_asm" != "no"],[
  AC_MSG_CHECKING(whether we can assemble basic amd64 code)
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  ]], [[
#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
/* neat */
#else
# error !amd64
#endif
__asm__("pxor %xmm12,%xmm6");
]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE_AMD64_ASM], [1], [basic amd64 code can be assembled])
   HAVE_AMD64_ASM_V=1],
  [AC_MSG_RESULT(no)])
])
AM_CONDITIONAL([HAVE_AMD64_ASM], [test $HAVE_AMD64_ASM_V = 1])
AC_SUBST(HAVE_AMD64_ASM_V)

AC_MSG_CHECKING(for 128-bit arithmetic)
HAVE_TI_MODE_V=0
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
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
]], [[
(void) fcontract;
]])],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_TI_MODE], [1], [gcc TI mode is available])
 HAVE_TI_MODE_V=1],
[AC_MSG_RESULT(no)])
AM_CONDITIONAL([HAVE_TI_MODE], [test $HAVE_TI_MODE_V = 1])
AC_SUBST(HAVE_TI_MODE_V)

AC_MSG_CHECKING(for cpuid instruction)
HAVE_CPUID_V=0
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ ]], [[
__asm__("movl %ebx,%esi\n"
        "cpuid\n"
        "movl %esi,%ebx");
]])],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_CPUID], [1], [cpuid instruction is available])
 HAVE_CPUID_V=1],
[AC_MSG_RESULT(no)])
AM_CONDITIONAL([HAVE_CPUID], [test $HAVE_CPUID_V = 1])
AC_SUBST(HAVE_CPUID_V)

AS_CASE([$host_cpu],
  [i*86 | x86_64 | powerpc* | s390*],
    [AC_MSG_NOTICE([data alignment is not required on this target])],
  [*],
    [AC_MSG_NOTICE([data alignment is required on this target])
     AC_DEFINE([CPU_ALIGNED_ACCESS_REQUIRED], [1], [data alignment is required])]
)

dnl Checks for functions and headers

AC_CHECK_FUNC(clock_gettime, , [AC_CHECK_LIB(rt, clock_gettime)])
AC_CHECK_FUNC(fegetenv, , [AC_CHECK_LIB(m, fegetenv)])

AC_CHECK_FUNCS([SecureZeroMemory arc4random arc4random_buf])

AC_SUBST([LIBTOOL_EXTRA_FLAGS])

dnl Libtool.

LT_INIT([dlopen])
AC_LIBTOOL_WIN32_DLL
gl_LD_OUTPUT_DEF

dnl Output.

AH_VERBATIM([NDEBUG], [/* Never ever ignore assertions */
#ifdef NDEBUG
#/**/undef/**/ NDEBUG
#endif])

AS_IF([test "x$PKG_CONFIG" != "x"], [
  AC_CONFIG_FILES([libsodium.pc])
])
AC_CONFIG_FILES([Makefile
                 msvc-scripts/Makefile
                 src/Makefile
                 src/libsodium/Makefile
                 src/libsodium/include/Makefile
                 src/libsodium/include/sodium/version.h
                 src/libsodium/include/sodium/crypto_scalarmult_curve25519.h
                 src/libsodium/include/sodium/crypto_stream_salsa20.h
                 test/default/Makefile
                 test/Makefile
                 ])
AC_OUTPUT
