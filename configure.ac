AC_PREREQ([2.61])
AC_INIT([libsodium],[0.3],[https://github.com/jedisct1/libsodium])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([libltdl/config])
AC_CONFIG_SRCDIR([src/libsodium/version.c])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AM_DEP_TRACK

AC_SUBST(VERSION)
ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

SODIUM_LIBRARY_VERSION=1:1:1
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
AC_SUBST(SODIUM_LIBRARY_VERSION)

LX_CFLAGS=${CFLAGS-NONE}
AC_PROG_CC_C99
AC_USE_SYSTEM_EXTENSIONS
CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"

AS_IF([test "$cross_compiling" != no],
  AC_DEFINE(CROSS_COMPILING,,[define if you are cross-compiling])
)

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

AX_CHECK_COMPILE_FLAG([-fcatch-undefined-c99-behavior],
  [CFLAGS="$CFLAGS -fcatch-undefined-c99-behavior"],
  [AX_CHECK_COMPILE_FLAG([-fwrapv], [CFLAGS="$CFLAGS -fwrapv"])]
)
AX_CHECK_COMPILE_FLAG([-fno-strict-aliasing], [CFLAGS="$CFLAGS -fno-strict-aliasing"])
AX_CHECK_COMPILE_FLAG([-fno-strict-overflow], [CFLAGS="$CFLAGS -fno-strict-overflow"])

LIBTOOL_EXTRA_FLAGS=-version-info $SODIUM_LIBRARY_VERSION
case $host_os in
  cygwin* | mingw* | pw32* | cegcc*)
    LIBTOOL_EXTRA_FLAGS=-module -avoid-version
    AX_CHECK_LINK_FLAG([-Wl,--dynamicbase], [LDFLAGS="$LDFLAGS -Wl,--dynamicbase"])
    AX_CHECK_LINK_FLAG([-Wl,--nxcompat], [LDFLAGS="$LDFLAGS -Wl,--nxcompat"])
    AX_CHECK_COMPILE_FLAG([-fstack-protector-all], [
      AX_CHECK_LINK_FLAG([-fstack-protector-all],
        [CFLAGS="$CFLAGS -fstack-protector-all"]
      )
    ])
  ;;
  dragonfly*)
    AX_CHECK_COMPILE_FLAG([-fstack-protector], [
      AX_CHECK_LINK_FLAG([-fstack-protector],
        [CFLAGS="$CFLAGS -fstack-protector"]
      )
    ])
  ;;
  *)
    AX_CHECK_COMPILE_FLAG([-fstack-protector-all], [
      AX_CHECK_LINK_FLAG([-fstack-protector-all],
        [CFLAGS="$CFLAGS -fstack-protector-all"]
      )
    ])
  ;;
esac

AX_CHECK_COMPILE_FLAG([-Wbounded], [CFLAGS="$CFLAGS -Wbounded"])
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
  [CWFLAGS="$CWFLAGS -Wno-unknown-warning-option"])],
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

AS_IF([test -d /usr/local/include], [
  CPPFLAGS="$CPPFLAGS -I/usr/local/include"
])

AS_IF([test -d /usr/local/lib], [
  LDFLAGS="$LDFLAGS -L/usr/local/lib"
])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_BIGENDIAN

dnl Checks for functions and headers

AC_CHECK_FUNCS([SecureZeroMemory])

dnl Switches.

AC_ARG_ENABLE(pie,
[AS_HELP_STRING(--enable-pie,Produce position independent executables @<:@default=yes@:>@)],
 enable_pie=$enableval, enable_pie="maybe")

AC_ARG_ENABLE(ssp,
[AS_HELP_STRING(--disable-ssp,Don't compile with -fstack-protector)],
[AS_IF([test "x$enableval" = "xno"], [
    nxflags=""
    for flag in `echo $CFLAGS`; do
      case "$flag" in
       -fstack-protector*) ;;
        *) AS_VAR_APPEND([nxflags], [" $flag"]) ;;
      esac
    done
    CFLAGS="$nxflags"
  ])
])

AC_ARG_ENABLE(blocking-random,
[AS_HELP_STRING(--enable-blocking-random,Use /dev/random instead of /dev/urandom)],
[AC_DEFINE([USE_BLOCKING_RANDOM], [], [Use blocking random])])

AC_ARG_ENABLE(debug,
[AS_HELP_STRING(--enable-debug,For maintainers only - please do not use)],
[
  AS_IF([test "x$LX_CFLAGS" = "xNONE"], [
    nxflags=""
    for flag in `echo $CFLAGS`; do
      case "$flag" in
        -O*) ;;
        -g*) ;;
        *) AS_VAR_APPEND([nxflags], [" $flag"]) ;;
      esac
    done
    CFLAGS="$nxflags -O0 -g3"
  ])
  CPPFLAGS="$CPPFLAGS -DDEBUG=1"
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

AC_SUBST([MAINT])
AC_SUBST([SODIUM_PATH])
AC_SUBST([LIBTOOL_EXTRA_FLAGS])

dnl Libtool.

LT_INIT([dlopen])
LT_CONFIG_LTDL_DIR([libltdl])
LTDL_INIT

dnl Output.

AH_VERBATIM([NDEBUG], [/* Never ever ignore assertions */
#ifdef NDEBUG
#/**/undef/**/ NDEBUG
#endif])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libsodium/Makefile
                 src/libsodium/include/Makefile
                 src/libsodium/include/sodium/version.h
                 test/Makefile
                 ])
AC_OUTPUT
