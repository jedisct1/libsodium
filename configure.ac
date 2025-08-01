AC_PREREQ([2.69])
AC_INIT([libsodium],[1.0.21],[https://github.com/jedisct1/libsodium/issues],[libsodium],[https://libsodium.org])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libsodium/sodium/version.c])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AM_DEP_TRACK

AC_SUBST(VERSION)

SODIUM_LIBRARY_VERSION_MAJOR=28
SODIUM_LIBRARY_VERSION_MINOR=0
DLL_VERSION=28
SODIUM_LIBRARY_VERSION=28:0:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                |        |        or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed
AC_SUBST(SODIUM_LIBRARY_VERSION_MAJOR)
AC_SUBST(SODIUM_LIBRARY_VERSION_MINOR)
AC_SUBST(SODIUM_LIBRARY_VERSION)
AC_SUBST(DLL_VERSION)

AC_LANG_ASSERT(C)
LX_CFLAGS=${CFLAGS-NONE}
PKGCONFIG_LIBS_PRIVATE=""

dnl Path check

AS_IF([pwd | fgrep ' ' > /dev/null 2>&1],
  [AC_MSG_ERROR([The build directory contains whitespaces - This can cause tests/installation to fail due to limitations of some libtool versions])]
)

sodium_CFLAGS=${CFLAGS+set}
: ${CFLAGS=""}

AC_PROG_CC
AM_PROG_AS
AC_USE_SYSTEM_EXTENSIONS

dnl Default optimization flags

if test "$sodium_CFLAGS" != "set" ; then
  AX_CHECK_COMPILE_FLAG([-O3], [CFLAGS="$CFLAGS -O3"],
    [AX_CHECK_COMPILE_FLAG([-O2], [CFLAGS="$CFLAGS -O2"],
      [AX_CHECK_COMPILE_FLAG([-O1], [CFLAGS="$CFLAGS -O1"],
        [AX_CHECK_COMPILE_FLAG([-O], [CFLAGS="$CFLAGS -O"])])])])
fi

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
[AS_HELP_STRING(--disable-asm,[Do not compile assembly code -- As a side effect, this disables CPU-specific implementations on non-Windows platforms. Only for use with targets such as WebAssembly.])],
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

AS_IF([test "x$EMSCRIPTEN" != "x"], [
  AX_CHECK_COMPILE_FLAG([-s ASSERTIONS=0], [
    enable_asm="no"
    AC_MSG_WARN([compiling to JavaScript - asm implementations disabled])
  ], [
    AC_MSG_WARN([EMSCRIPTEN environment variable defined, but emcc doesn\'t appear to be used - Assuming compilation to native code])
    CFLAGS="$CFLAGS -U__EMSCRIPTEN__"
    unset EMSCRIPTEN
  ])
])

AC_ARG_ENABLE(pie,
[AS_HELP_STRING(--disable-pie,Do not produce position independent executables)],
 enable_pie=$enableval, enable_pie="maybe")

AS_CASE([$host_os], [mingw*|cygwin*|msys|eabi*], [enable_pie="no"])

AC_ARG_ENABLE(blocking-random,
[AS_HELP_STRING(--enable-blocking-random,Enable this switch only if /dev/urandom is totally broken on the target platform)],
[
  AS_IF([test "x$enableval" = "xyes"], [
    AC_DEFINE([USE_BLOCKING_RANDOM], [1], [/dev/urandom is insecure on the target platform])
  ])
])

AC_ARG_ENABLE(minimal,
[AS_HELP_STRING(--enable-minimal,
  [Only compile the minimum set of functions required for the high-level API])],
[
  AS_IF([test "x$enableval" = "xyes"], [
    enable_minimal="yes"
    SODIUM_LIBRARY_MINIMAL_DEF="#define SODIUM_LIBRARY_MINIMAL 1"
    AC_DEFINE([MINIMAL], [1], [Define for a minimal build, without deprecated functions and functions that high-level APIs depend on])
  ], [
    enable_minimal="no"
  ])
],
[
  enable_minimal="no"
])
AM_CONDITIONAL([MINIMAL], [test x$enable_minimal = xyes])
AC_SUBST(SODIUM_LIBRARY_MINIMAL_DEF)

AC_ARG_WITH(pthreads, AS_HELP_STRING([--with-pthreads],
 [use pthreads library, or --without-pthreads to disable threading support]),
 [ ], [withval="yes"])

AS_IF([test "x$withval" = "xyes"], [
  AX_PTHREAD([
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <pthread.h>
      ]], [[
        pthread_mutex_t mutex;

        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex)
      ]]
    )], [
        AC_DEFINE([HAVE_PTHREAD], [1], [Define if you have POSIX threads libraries and header files])
        with_threads="yes"
        LIBS="$PTHREAD_LIBS $LIBS"
        PKGCONFIG_LIBS_PRIVATE="$PTHREAD_LIBS $PTHREAD_CFLAGS $PKGCONFIG_LIBS_PRIVATE"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        CC="$PTHREAD_CC"
      ])
    ],
    [ AC_MSG_NOTICE(pthread mutexes are not available) ]
  )
], [with_threads="no"])

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

AC_ARG_WITH(ctgrind,
[AS_HELP_STRING(--with-ctgrind,For maintainers only - please do not use)],
[AS_IF([test "x$withval" = "xyes"], [
    AC_CHECK_LIB(ctgrind, ct_poison)
  ])
])

AC_ARG_ENABLE(retpoline,
[AS_HELP_STRING(--enable-retpoline,Use return trampolines for indirect calls)],
[AS_IF([test "x$enableval" = "xyes"], [
  AX_CHECK_COMPILE_FLAG([-mindirect-branch=thunk-inline],
    [CFLAGS="$CFLAGS -mindirect-branch=thunk-inline"],
    [AX_CHECK_COMPILE_FLAG([-mretpoline], [CFLAGS="$CFLAGS -mretpoline"])]
  )
  AX_CHECK_COMPILE_FLAG([-mindirect-branch-register])
  ])
])

ENABLE_CWFLAGS=no
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
      CFLAGS="$nxflags -O -g3"
    ])
    ENABLE_CWFLAGS=yes
    CPPFLAGS="$CPPFLAGS -DDEBUG=1 -U_FORTIFY_SOURCE"
  ])
])

AC_ARG_ENABLE(opt,
[AS_HELP_STRING(--enable-opt,Optimize for the native CPU - The resulting library will be faster but not portable)],
[
  AS_IF([test "x$enableval" = "xyes"], [
    AX_CHECK_COMPILE_FLAG([-ftree-vectorize], [CFLAGS="$CFLAGS -ftree-vectorize"])
    AX_CHECK_COMPILE_FLAG([-ftree-slp-vectorize], [CFLAGS="$CFLAGS -ftree-slp-vectorize"])
    AX_CHECK_COMPILE_FLAG([-fomit-frame-pointer], [CFLAGS="$CFLAGS -fomit-frame-pointer"])
    AX_CHECK_COMPILE_FLAG([-march=native], [CFLAGS="$CFLAGS -march=native"])
    AX_CHECK_COMPILE_FLAG([-mtune=native], [CFLAGS="$CFLAGS -mtune=native"])
  ])
])

AC_SUBST(MAINT)
AC_SUBST(PKGCONFIG_LIBS_PRIVATE)

AX_VALGRIND_CHECK

dnl Checks

AC_C_VARARRAYS

AC_CHECK_DEFINE([__wasi__], [WASI="yes"], [])

AS_CASE([$host_os], [linux-gnu], [AX_ADD_FORTIFY_SOURCE], [ ])

AX_CHECK_COMPILE_FLAG([-fvisibility=hidden],
  [CFLAGS="$CFLAGS -fvisibility=hidden"])

AS_CASE([$host_os], [cygwin*|mingw*|msys|pw32*|cegcc*|eabi*], [ ], [
  AX_CHECK_COMPILE_FLAG([-fPIC], [CFLAGS="$CFLAGS -fPIC"])
])

AS_IF([test "$enable_pie" != "no"],[
  AX_CHECK_COMPILE_FLAG([-fPIE], [
    AX_CHECK_LINK_FLAG([-pie], [
      [CFLAGS="$CFLAGS -fPIE"
       LDFLAGS="$LDFLAGS -pie"]
    ])
  ])
])

AX_CHECK_COMPILE_FLAG([-fno-strict-aliasing], [CFLAGS="$CFLAGS -fno-strict-aliasing"])
AX_CHECK_COMPILE_FLAG([-fno-strict-overflow], [CFLAGS="$CFLAGS -fno-strict-overflow"], [
  AX_CHECK_COMPILE_FLAG([-fwrapv], [CFLAGS="$CFLAGS -fwrapv"])
])

AS_IF([test "$GCC" = "yes" ], [
  AS_CASE([$host_cpu],
    [i?86|amd64|x86_64], [
      AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE([
#if !defined(__clang__) && defined(__GNUC__) && ((__GNUC__ << 8) | __GNUC_MINOR__) < 0x403
# error old gcc
#endif
int main(void) { return 0; }
         ])],,[
          AX_CHECK_COMPILE_FLAG([-flax-vector-conversions], [CFLAGS="$CFLAGS -flax-vector-conversions"])
        ])
      ]
    )
  ])

LIBTOOL_OLD_FLAGS="$LIBTOOL_EXTRA_FLAGS"
LIBTOOL_EXTRA_FLAGS="$LIBTOOL_EXTRA_FLAGS -version-info $SODIUM_LIBRARY_VERSION"
AC_ARG_ENABLE(soname-versions,
  [AS_HELP_STRING([--enable-soname-versions], [enable soname versions (must be disabled for Android) (default: enabled)])],
    [
        AS_IF([test "x$enableval" = "xno"], [
          LIBTOOL_EXTRA_FLAGS="$LIBTOOL_OLD_FLAGS -avoid-version"
        ])
    ]
)

AS_CASE([$host_os],
  [cygwin*|mingw*|msys|pw32*|cegcc*], [
    AX_CHECK_LINK_FLAG([-Wl,--dynamicbase], [LDFLAGS="$LDFLAGS -Wl,--dynamicbase"])
    AX_CHECK_LINK_FLAG([-Wl,--high-entropy-va], [LDFLAGS="$LDFLAGS -Wl,--high-entropy-va"])
    AX_CHECK_LINK_FLAG([-Wl,--nxcompat], [LDFLAGS="$LDFLAGS -Wl,--nxcompat"])
  ])

AS_CASE([$host_os],
  [cygwin*|mingw*|msys|pw32*|cegcc*|eabi*], [
    AX_CHECK_COMPILE_FLAG([-fno-asynchronous-unwind-tables], [
      [CFLAGS="$CFLAGS -fno-asynchronous-unwind-tables"]
    ])
])

AS_IF([test "x$enable_ssp" != "xno"],[

AS_CASE([$host_os],
  [solaris*|cygwin*|mingw*|msys|pw32*|cegcc*|haiku|none|eabi*], [ ],
  [*], [
    AX_CHECK_COMPILE_FLAG([-fstack-protector], [
      AX_CHECK_LINK_FLAG([-fstack-protector],
        [CFLAGS="$CFLAGS -fstack-protector"]
      )
    ])
  ])
])

AX_CHECK_COMPILE_FLAG([$CFLAGS -Wall], [CWFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([$CFLAGS -Wno-deprecated-declarations], [CFLAGS="$CFLAGS -Wno-deprecated-declarations"])
AX_CHECK_COMPILE_FLAG([$CFLAGS -Wno-unknown-pragmas], [CFLAGS="$CFLAGS -Wno-unknown-pragmas"])

AC_ARG_VAR([CWFLAGS], [compilation flags for generating extra warnings])

AC_MSG_CHECKING(for clang)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[
#ifndef __clang__
#error Not clang
#endif
]])],
  [AC_MSG_RESULT(yes)
   AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wno-unknown-warning-option],
     [CWFLAGS="$CWFLAGS -Wno-unknown-warning-option"])
  ],
  [AC_MSG_RESULT(no)
])

AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wextra], [CWFLAGS="$WCFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Warray-bounds], [CWFLAGS="$CWFLAGS -Warray-bounds"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wbad-function-cast], [CWFLAGS="$CWFLAGS -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcast-qual], [CWFLAGS="$CWFLAGS -Wcast-qual"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wdiv-by-zero], [CWFLAGS="$CWFLAGS -Wdiv-by-zero"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wduplicated-branches], [CWFLAGS="$CWFLAGS -Wduplicated-branches"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wduplicated-cond], [CWFLAGS="$CWFLAGS -Wduplicated-cond"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wfloat-equal], [CWFLAGS="$CWFLAGS -Wfloat-equal"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wformat=2], [CWFLAGS="$CWFLAGS -Wformat=2"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wlogical-op], [CWFLAGS="$CWFLAGS -Wlogical-op"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmaybe-uninitialized], [CWFLAGS="$CWFLAGS -Wmaybe-uninitialized"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmisleading-indentation], [CWFLAGS="$CWFLAGS -Wmisleading-indentation"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-declarations], [CWFLAGS="$CWFLAGS -Wmissing-declarations"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-prototypes], [CWFLAGS="$CWFLAGS -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wnested-externs], [CWFLAGS="$CWFLAGS -Wnested-externs"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wno-type-limits], [CWFLAGS="$CWFLAGS -Wno-type-limits"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wno-unknown-pragmas], [CWFLAGS="$CWFLAGS -Wno-unknown-pragmas"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wnormalized=id], [CWFLAGS="$CWFLAGS -Wnormalized=id"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wnull-dereference], [CWFLAGS="$CWFLAGS -Wnull-dereference"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wold-style-declaration], [CWFLAGS="$CWFLAGS -Wold-style-declaration"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wpointer-arith], [CWFLAGS="$CWFLAGS -Wpointer-arith"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wredundant-decls], [CWFLAGS="$CWFLAGS -Wredundant-decls"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wrestrict], [CWFLAGS="$CWFLAGS -Wrestrict"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wshorten-64-to-32], [CWFLAGS="$CWFLAGS -Wshorten-64-to-32"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wsometimes-uninitialized], [CWFLAGS="$CWFLAGS -Wsometimes-uninitialized"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wstrict-prototypes], [CWFLAGS="$CWFLAGS -Wstrict-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wswitch-enum], [CWFLAGS="$CWFLAGS -Wswitch-enum"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wvariable-decl], [CWFLAGS="$CWFLAGS -Wvariable-decl"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wvla], [CWFLAGS="$WCFLAGS -Wvla"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wwrite-strings], [CWFLAGS="$CWFLAGS -Wwrite-strings"])

AS_IF([test "x$EMSCRIPTEN" = "x"], [
  AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS="$LDFLAGS -Wl,-z,relro"])
  AX_CHECK_LINK_FLAG([-Wl,-z,now], [LDFLAGS="$LDFLAGS -Wl,-z,now"])
  AX_CHECK_LINK_FLAG([-Wl,-z,noexecstack], [LDFLAGS="$LDFLAGS -Wl,-z,noexecstack"])
])

AX_CHECK_CATCHABLE_SEGV
AX_CHECK_CATCHABLE_ABRT

AS_IF([test "x$with_threads" = "xyes"], [
  AX_TLS([AC_MSG_RESULT(thread local storage is supported)
          AX_CHECK_COMPILE_FLAG([-ftls-model=local-dynamic],
            [CFLAGS="$CFLAGS -ftls-model=local-dynamic"])],
         [AC_MSG_RESULT(thread local storage is not supported)]) ])

LT_INIT
AC_SUBST(LIBTOOL_DEPS)

AC_ARG_VAR([AR], [path to the ar utility])
AC_CHECK_TOOL([AR], [ar], [ar])

dnl Checks for headers and codegen feature flags

target_cpu_aarch64=no
AC_MSG_CHECKING(for ARM64 target)
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([
#ifndef __aarch64__
#error Not aarch64
#endif
#include <arm_neon.h>
   ], [(void) 0])],
   [AC_MSG_RESULT(yes)
    target_cpu_aarch64=yes],
   [AC_MSG_RESULT(no)
    target_cpu_aarch64=no])

AS_IF([test "x$EMSCRIPTEN" = "x"], [

  AS_IF([test "x$target_cpu_aarch64" = "xyes"], [
    have_armcrypto=no
    AC_MSG_CHECKING(for ARM crypto instructions set)
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
      #ifndef __ARM_FEATURE_CRYPTO
      #  define __ARM_FEATURE_CRYPTO 1
      #endif
      #ifndef __ARM_FEATURE_AES
      #  define __ARM_FEATURE_AES 1
      #endif

      #include <arm_neon.h>

      #ifdef __clang__
      #  pragma clang attribute push(__attribute__((target("neon,crypto,aes"))), apply_to = function)
      #elif defined(__GNUC__)
      #  pragma GCC target("+simd+crypto")
      #endif
    ]], [[
      int64x2_t x = { 0, 0 };
      vaeseq_u8(vmovq_n_u8(0), vmovq_n_u8(0));
      vmull_high_p64(vreinterpretq_p64_s64(x), vreinterpretq_p64_s64(x));

      #ifdef __clang__
      #  pragma clang attribute pop
      #endif
      (void) 0
      ]])],
      [
        AC_MSG_RESULT(yes)
        have_armcrypto=yes
      ],
      [
        AC_MSG_RESULT(no)
        oldcflags="$CFLAGS"
        AX_CHECK_COMPILE_FLAG([-march=armv8-a+crypto+aes], [
          CFLAGS="$CFLAGS -march=armv8-a+crypto+aes"
          AC_MSG_CHECKING(for ARM crypto instructions set with -march=armv8-a+crypto+aes)
          AC_LINK_IFELSE([AC_LANG_PROGRAM([[
            #ifdef __clang__
            #  pragma clang attribute push(__attribute__((target("neon,crypto,aes"))), apply_to = function)
            #elif defined(__GNUC__)
            #  pragma GCC target("+simd+crypto")
            #endif
            #ifndef __ARM_FEATURE_CRYPTO
            #  define __ARM_FEATURE_CRYPTO 1
            #endif
            #ifndef __ARM_FEATURE_AES
            #  define __ARM_FEATURE_AES 1
            #endif

            #include <arm_neon.h>
          ]], [[
            int64x2_t x = { 0, 0 };
            vaeseq_u8(vmovq_n_u8(0), vmovq_n_u8(0));
            vmull_high_p64(vreinterpretq_p64_s64(x), vreinterpretq_p64_s64(x));

            #ifdef __clang__
            #  pragma clang attribute pop
            #endif
            (void) 0
          ]])],
            [
              AC_MSG_RESULT(yes - with addition of -march=armv8-a+crypto+aes)
              have_armcrypto=yes
              CFLAGS_ARMCRYPTO="-march=armv8-a+crypto+aes"
            ],
            [AC_MSG_RESULT(no)])
          CFLAGS="$oldcflags"
        ])
      ])
      AS_IF([test "$have_armcrypto" = "yes"],[AC_DEFINE([HAVE_ARMCRYPTO], [1], [ARM crypto extensions are available])])
  ])

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mmmx], [CFLAGS="$CFLAGS -mmmx"])
  AC_MSG_CHECKING(for MMX instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("mmx")
#include <mmintrin.h>
]], [[ __m64 x = _mm_setzero_si64(); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_MMINTRIN_H], [1], [mmx is available])
     AX_CHECK_COMPILE_FLAG([-mmmx], [CFLAGS_MMX="-mmmx"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-msse2], [CFLAGS="$CFLAGS -msse2"])
  AC_MSG_CHECKING(for SSE2 instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("sse2")
#ifndef __SSE2__
# define __SSE2__
#endif
#include <emmintrin.h>
]], [[ __m128d x = _mm_setzero_pd();
       __m128i z = _mm_srli_epi64(_mm_setzero_si128(), 26); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_EMMINTRIN_H], [1], [sse2 is available])
     AX_CHECK_COMPILE_FLAG([-msse2], [CFLAGS_SSE2="-msse2"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-msse3], [CFLAGS="$CFLAGS -msse3"])
  AC_MSG_CHECKING(for SSE3 instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("sse3")
#include <pmmintrin.h>
]], [[ __m128 x = _mm_addsub_ps(_mm_cvtpd_ps(_mm_setzero_pd()),
                                _mm_cvtpd_ps(_mm_setzero_pd())); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_PMMINTRIN_H], [1], [sse3 is available])
     AX_CHECK_COMPILE_FLAG([-msse3], [CFLAGS_SSE3="-msse3"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mssse3], [CFLAGS="$CFLAGS -mssse3"])
  AC_MSG_CHECKING(for SSSE3 instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("ssse3")
#include <tmmintrin.h>
]], [[ __m64 x = _mm_abs_pi32(_m_from_int(0)); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_TMMINTRIN_H], [1], [ssse3 is available])
     AX_CHECK_COMPILE_FLAG([-mssse3], [CFLAGS_SSSE3="-mssse3"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-msse4.1], [CFLAGS="$CFLAGS -msse4.1"])
  AC_MSG_CHECKING(for SSE4.1 instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("sse4.1")
#include <smmintrin.h>
]], [[ __m128i x = _mm_minpos_epu16(_mm_setzero_si128()); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_SMMINTRIN_H], [1], [sse4.1 is available])
     AX_CHECK_COMPILE_FLAG([-msse4.1], [CFLAGS_SSE41="-msse4.1"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mavx], [CFLAGS="$CFLAGS -mavx"])
  AC_MSG_CHECKING(for AVX instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("avx")
#include <immintrin.h>
]], [[ _mm256_zeroall(); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_AVXINTRIN_H], [1], [AVX is available])
     AX_CHECK_COMPILE_FLAG([-mavx], [CFLAGS_AVX="-mavx"])],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mavx2], [CFLAGS="$CFLAGS -mavx2"])
  AC_MSG_CHECKING(for AVX2 instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("avx2")
#include <immintrin.h>
]], [[
__m256 x = _mm256_set1_ps(3.14);
__m256 y = _mm256_permutevar8x32_ps(x, _mm256_set1_epi32(42));
return _mm256_movemask_ps(_mm256_cmp_ps(x, y, _CMP_NEQ_OQ));
]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_AVX2INTRIN_H], [1], [AVX2 is available])
     AX_CHECK_COMPILE_FLAG([-mavx2], [CFLAGS_AVX2="-mavx2"])
     AC_MSG_CHECKING(if _mm256_broadcastsi128_si256 is correctly defined)
     AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("avx2")
#include <immintrin.h>
     ]], [[ __m256i y = _mm256_broadcastsi128_si256(_mm_setzero_si128()); ]])],
       [AC_MSG_RESULT(yes)],
       [AC_MSG_RESULT(no)
        AC_DEFINE([_mm256_broadcastsi128_si256], [_mm_broadcastsi128_si256],
                  [Define to the local name of _mm256_broadcastsi128_si256])])
     ],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mavx512f], [CFLAGS="$CFLAGS -mavx512f"])
  AC_MSG_CHECKING(for AVX512F instructions set)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("avx512f")
#include <immintrin.h>
]], [[

#ifndef __AVX512F__
# error No AVX512 support
#elif defined(__clang__)
# if __clang_major__ < 4
#  error Compiler AVX512 support may be broken
# endif
#elif defined(__GNUC__)
# if __GNUC__ < 6
#  error Compiler AVX512 support may be broken
# endif
#endif

__m512i x = _mm512_setzero_epi32();
__m512i y = _mm512_permutexvar_epi64(_mm512_setr_epi64(0, 1, 4, 5, 2, 3, 6, 7), x);
]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_AVX512FINTRIN_H], [1], [AVX512F is available])
     AX_CHECK_COMPILE_FLAG([-mavx512f], [CFLAGS_AVX512F="-mavx512f"])],
    [AC_MSG_RESULT(no)
     AX_CHECK_COMPILE_FLAG([$CFLAGS -mno-avx512f],
       [CFLAGS="$CFLAGS -mno-avx512f"])
    ])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-maes], [CFLAGS="$CFLAGS -maes"])
  AX_CHECK_COMPILE_FLAG([-mpclmul], [CFLAGS="$CFLAGS -mpclmul"])
  AC_MSG_CHECKING(for AESNI instructions set and PCLMULQDQ)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("aes")
#pragma GCC target("pclmul")
#include <wmmintrin.h>
]], [[ __m128i x = _mm_aesimc_si128(_mm_setzero_si128());
       __m128i y = _mm_clmulepi64_si128(_mm_setzero_si128(), _mm_setzero_si128(), 0);]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_WMMINTRIN_H], [1], [aesni is available])
     AX_CHECK_COMPILE_FLAG([-maes], [CFLAGS_AESNI="-maes"])
     AX_CHECK_COMPILE_FLAG([-mpclmul], [CFLAGS_PCLMUL="-mpclmul"])
     ],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

  oldcflags="$CFLAGS"
  AX_CHECK_COMPILE_FLAG([-mrdrnd], [CFLAGS="$CFLAGS -mrdrnd"])
  AC_MSG_CHECKING(for RDRAND)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#pragma GCC target("rdrnd")
#include <immintrin.h>
]], [[ unsigned long long x; _rdrand64_step(&x); ]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE([HAVE_RDRAND], [1], [rdrand is available])
     AX_CHECK_COMPILE_FLAG([-mrdrnd], [CFLAGS_RDRAND="-mrdrnd"])
     ],
    [AC_MSG_RESULT(no)])
  CFLAGS="$oldcflags"

])

AC_SUBST(CFLAGS_ARMCRYPTO)
AC_SUBST(CFLAGS_MMX)
AC_SUBST(CFLAGS_SSE2)
AC_SUBST(CFLAGS_SSE3)
AC_SUBST(CFLAGS_SSSE3)
AC_SUBST(CFLAGS_SSE41)
AC_SUBST(CFLAGS_AVX)
AC_SUBST(CFLAGS_AVX2)
AC_SUBST(CFLAGS_AVX512F)
AC_SUBST(CFLAGS_AESNI)
AC_SUBST(CFLAGS_PCLMUL)
AC_SUBST(CFLAGS_RDRAND)

AC_CHECK_HEADERS([sys/mman.h sys/param.h sys/random.h intrin.h sys/auxv.h])
AC_CHECK_HEADERS([CommonCrypto/CommonRandom.h])
AC_CHECK_HEADERS([cet.h threads.h])

AC_MSG_CHECKING([if _xgetbv() is available])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[ #include <intrin.h> ]], [[ (void) _xgetbv(0) ]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE__XGETBV], [1], [_xgetbv() is available])],
  [AC_MSG_RESULT(no)])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_INLINE
AS_CASE([$host_cpu],
  [i?86|amd64|x86_64],
    [ac_cv_c_bigendian=no]
)
AC_C_BIGENDIAN(
  AC_DEFINE(NATIVE_BIG_ENDIAN, 1, [machine is bigendian]),
  AC_DEFINE(NATIVE_LITTLE_ENDIAN, 1, [machine is littleendian]),
  AC_MSG_ERROR([unknown endianness]),
  AC_MSG_ERROR([universal endianness is not supported - compile separately and use lipo(1)])
)

AC_MSG_CHECKING(whether __STDC_LIMIT_MACROS is required)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
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

AC_MSG_CHECKING(whether we can use inline asm code)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
]], [[
int a = 42;
int *pnt = &a;
__asm__ __volatile__ ("" : : "r"(pnt) : "memory");
]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE_INLINE_ASM], [1], [inline asm code can be used])]
  [AC_MSG_RESULT(no)]
)

HAVE_AMD64_ASM_V=0
AS_IF([test "$enable_asm" != "no"],[
  AC_MSG_CHECKING(whether we can use x86_64 asm code)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  ]], [[
#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
# if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || defined(_WIN64) || defined(__midipix__)
#  error Windows x86_64 calling conventions are not supported yet
# endif
/* neat */
#else
# error !x86_64
#endif
unsigned char i = 0, o = 0, t;
__asm__ __volatile__ ("pxor %%xmm12, %%xmm6 \n"
                      "movb (%[i]), %[t] \n"
                      "addb %[t], (%[o]) \n"
                      : [t] "=&r"(t)
                      : [o] "D"(&o), [i] "S"(&i)
                      : "memory", "flags", "cc");
]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE_AMD64_ASM], [1], [x86_64 asm code can be used])
   HAVE_AMD64_ASM_V=1],
  [AC_MSG_RESULT(no)])
])
AM_CONDITIONAL([HAVE_AMD64_ASM], [test $HAVE_AMD64_ASM_V = 1])
AC_SUBST(HAVE_AMD64_ASM_V)

HAVE_AVX_ASM_V=0
AS_IF([test "$enable_asm" != "no"],[
  AC_MSG_CHECKING(whether we can assemble AVX opcodes)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  ]], [[
#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
# if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || defined(_WIN64)
#  error Windows x86_64 calling conventions are not supported yet
# endif
/* neat */
#else
# error !x86_64
#endif
__asm__ __volatile__ ("vpunpcklqdq %xmm0,%xmm13,%xmm0");
]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE_AVX_ASM], [1], [AVX opcodes are supported])
   HAVE_AVX_ASM_V=1],
  [AC_MSG_RESULT(no)])
])
AM_CONDITIONAL([HAVE_AVX_ASM], [test $HAVE_AVX_ASM_V = 1])
AC_SUBST(HAVE_AVX_ASM_V)

AC_MSG_CHECKING(for 128-bit arithmetic)
HAVE_TI_MODE_V=0
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if !defined(__clang__) && !defined(__GNUC__) && !defined(__SIZEOF_INT128__)
# error mode(TI) is a gcc extension, and __int128 is not available
#endif
#if defined(__clang__) && !defined(__x86_64__) && !defined(__aarch64__)
# error clang does not properly handle the 128-bit type on 32-bit systems
#endif
#ifndef NATIVE_LITTLE_ENDIAN
# error libsodium currently expects a little endian CPU for the 128-bit type
#endif
#ifdef __EMSCRIPTEN__
# error emscripten currently doesn't support some operations on integers larger than 64 bits
#endif
#include <stddef.h>
#include <stdint.h>
#if defined(__SIZEOF_INT128__)
typedef unsigned __int128 uint128_t;
#else
typedef unsigned uint128_t __attribute__((mode(TI)));
#endif
void fcontract(uint128_t *t) {
  *t += 0x8000000000000 - 1;
  *t *= *t;
  *t >>= 84;
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

HAVE_CPUID_V=0
AS_IF([test "$enable_asm" != "no"],[
  AC_MSG_CHECKING(for cpuid instruction)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[ ]], [[
unsigned int cpu_info[4];
__asm__ __volatile__ ("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1" :
                      "=a" (cpu_info[0]), "=&r" (cpu_info[1]),
                      "=c" (cpu_info[2]), "=d" (cpu_info[3]) :
                      "0" (0U), "2" (0U));
  ]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([HAVE_CPUID], [1], [cpuid instruction is available])
   HAVE_CPUID_V=1],
  [AC_MSG_RESULT(no)])
  ])
AC_SUBST(HAVE_CPUID_V)

asm_hide_symbol="unsupported"
AS_IF([test "$enable_asm" != "no"],[
  AC_MSG_CHECKING(if the .private_extern asm directive is supported)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[ ]], [[
__asm__ __volatile__ (".private_extern dummy_symbol \n"
                      ".private_extern _dummy_symbol \n"
                      ".globl dummy_symbol \n"
                      ".globl _dummy_symbol \n"
                      "dummy_symbol: \n"
                      "_dummy_symbol: \n"
                      "    nop \n"
);
  ]])],
  [AC_MSG_RESULT(yes)
   asm_hide_symbol=".private_extern"],
  [AC_MSG_RESULT(no)])

  AC_MSG_CHECKING(if the .hidden asm directive is supported)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[ ]], [[
__asm__ __volatile__ (".hidden dummy_symbol \n"
                      ".hidden _dummy_symbol \n"
                      ".globl dummy_symbol \n"
                      ".globl _dummy_symbol \n"
                      "dummy_symbol: \n"
                      "_dummy_symbol: \n"
                      "    nop \n"
);
  ]])],
  [AC_MSG_RESULT(yes)
   AS_IF([test "$asm_hide_symbol" = "unsupported"],
          [asm_hide_symbol=".hidden"],
          [AC_MSG_NOTICE([unable to reliably tag symbols as private])
           asm_hide_symbol="unsupported"])
  ],
  [AC_MSG_RESULT(no)])

  AS_IF([test "$asm_hide_symbol" != "unsupported"],[
    AC_DEFINE_UNQUOTED([ASM_HIDE_SYMBOL], [$asm_hide_symbol], [directive to hide symbols])
  ])
])

AC_MSG_CHECKING(if weak symbols are supported)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#if !defined(__ELF__) && !defined(__APPLE_CC__)
# error Support for weak symbols may not be available
#endif
__attribute__((weak)) void __dummy(void *x) { }
void f(void *x) { __dummy(x); }
]], [[ ]]
)],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_WEAK_SYMBOLS], [1], [weak symbols are supported])],
[AC_MSG_RESULT(no)])

AC_MSG_CHECKING(if atomic operations are supported)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[ ]], [[
static volatile int _sodium_lock;
__sync_lock_test_and_set(&_sodium_lock, 1);
__sync_lock_release(&_sodium_lock);
]]
)],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_ATOMIC_OPS], [1], [atomic operations are supported])],
[AC_MSG_RESULT(no)])

AC_MSG_CHECKING(if C11 memory fences are supported)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdatomic.h>
 ]], [[
atomic_thread_fence(memory_order_acquire);
]]
)],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_C11_MEMORY_FENCES], [1], [C11 memory fences are supported])],
[AC_MSG_RESULT(no)])

AC_MSG_CHECKING(if gcc memory fences are supported)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[ ]], [[
__atomic_thread_fence(__ATOMIC_ACQUIRE);
]]
)],
[AC_MSG_RESULT(yes)
 AC_DEFINE([HAVE_GCC_MEMORY_FENCES], [1], [GCC memory fences are supported])],
[AC_MSG_RESULT(no)])

dnl Checks for functions and headers

AC_FUNC_ALLOCA
AS_IF([test "x$EMSCRIPTEN" = "x"],[
  AC_CHECK_FUNCS([arc4random arc4random_buf])
  AS_IF([test "x$WASI" = "x"],[
    AC_CHECK_FUNCS([mmap mlock madvise mprotect])
    AC_CHECK_FUNCS([raise])
    AC_CHECK_FUNCS([sysconf])
  ])
  AC_MSG_CHECKING(for getrandom with a standard API)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_RANDOM_H
# include <sys/random.h>
#endif
]], [[
unsigned char buf;
if (&getrandom != NULL) {
  (void) getrandom((void *) &buf, 1U, 0U);
}
  ]])],
  [AC_MSG_RESULT(yes)
   AC_CHECK_FUNCS([getrandom])],
  [AC_MSG_RESULT(no)
  ])

  AC_MSG_CHECKING(for getentropy with a standard API)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_RANDOM_H
# include <sys/random.h>
#endif
]], [[
unsigned char buf;
if (&getentropy != NULL) {
  (void) getentropy((void *) &buf, 1U);
}
  ]])],
  [AC_MSG_RESULT(yes)
   AC_CHECK_FUNCS([getentropy])],
  [AC_MSG_RESULT(no)
  ])
])

AS_IF([test "x$WASI" = "x"],[
  AC_CHECK_FUNCS([getpid])
  AC_CHECK_FUNCS([getauxval elf_aux_info])
])

AC_CHECK_FUNCS([posix_memalign nanosleep clock_gettime])

AS_IF([test "x$WASI" = "x"],[
  AC_CHECK_FUNCS([memset_s explicit_bzero memset_explicit explicit_memset])
])

AC_SUBST([LIBTOOL_EXTRA_FLAGS])

TEST_LDFLAGS=''
AS_IF([test "x$EMSCRIPTEN" != "x"],[
  EXEEXT=.js
  TEST_LDFLAGS='--pre-js pre.js.inc -s RESERVED_FUNCTION_POINTERS=8'
])
AC_SUBST(TEST_LDFLAGS)
AM_CONDITIONAL([EMSCRIPTEN], [test "x$EMSCRIPTEN" != "x"])
AM_CONDITIONAL([WASI], [test "x$WASI" != "x"])

AC_DEFINE([CONFIGURED], [1], [the build system was properly configured])

dnl Libtool.

LT_INIT([dlopen win32-dll])
gl_LD_OUTPUT_DEF

dnl Output.

AH_VERBATIM([NDEBUG], [/* Always evaluate assert() calls */
#ifdef NDEBUG
#/**/undef/**/ NDEBUG
#endif])

AS_IF([test "x$ENABLE_CWFLAGS" = "xyes"], [
  CFLAGS="$CFLAGS $CWFLAGS"
])

AC_CONFIG_FILES([Makefile
                 builds/Makefile
                 dist-build/Makefile
                 libsodium.pc
                 libsodium-uninstalled.pc
                 src/Makefile
                 src/libsodium/Makefile
                 src/libsodium/include/Makefile
                 src/libsodium/include/sodium/version.h
                 test/default/Makefile
                 test/Makefile
                 ])
AC_OUTPUT
