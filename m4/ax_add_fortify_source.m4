# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_add_fortify_source.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_ADD_FORTIFY_SOURCE
#
# DESCRIPTION
#
#   Check whether -D_FORTIFY_SOURCE=2 can be added to CPPFLAGS without macro
#   redefinition warnings, other cpp warnings or linker. Some distributions
#   (such as Ubuntu or Gentoo Linux) enable _FORTIFY_SOURCE globally in
#   their compilers, leading to unnecessary warnings in the form of
#
#     <command-line>:0:0: error: "_FORTIFY_SOURCE" redefined [-Werror]
#     <built-in>: note: this is the location of the previous definition
#
#   which is a problem if -Werror is enabled. This macro checks whether
#   _FORTIFY_SOURCE is already defined, and if not, adds -D_FORTIFY_SOURCE=2
#   to CPPFLAGS.
#
#   Newer mingw-w64 msys2 package comes with a bug in
#   headers-git-7.0.0.5546.d200317d-1. It broke -D_FORTIFY_SOURCE support,
#   and would need -lssp or -fstack-protector.  See
#   https://github.com/msys2/MINGW-packages/issues/5803. Try to actually
#   link it.
#
# LICENSE
#
#   Copyright (c) 2017 David Seifert <soap@gentoo.org>
#   Copyright (c) 2019, 2023 Reini Urban <rurban@cpan.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_ADD_FORTIFY_SOURCE],[
    ac_save_cflags=$CFLAGS
    ac_cwerror_flag=yes
    AX_CHECK_COMPILE_FLAG([-Werror],[CFLAGS="$CFLAGS -Werror"])
    ax_add_fortify_3_failed=
    AC_MSG_CHECKING([whether to add -D_FORTIFY_SOURCE=3 to CPPFLAGS])
    AC_LINK_IFELSE([
        AC_LANG_PROGRAM([],
            [[
                #ifndef _FORTIFY_SOURCE
                    return 0;
                #else
                    _FORTIFY_SOURCE_already_defined;
                #endif
            ]]
        )],
        AC_LINK_IFELSE([
            AC_LANG_SOURCE([[
                #define _FORTIFY_SOURCE 3
                #include <string.h>
                int main() {
                    char *s = " ";
                    strcpy(s, "x");
                    return strlen(s)-1;
                }
              ]]
            )],
            [
              AC_MSG_RESULT([yes])
              CFLAGS=$ac_save_cflags
              CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=3"
            ], [
              AC_MSG_RESULT([no])
              CFLAGS=$ac_save_cflags
              ax_add_fortify_3_failed=1
            ],
        ),
        [
          AC_MSG_RESULT([no])
          CFLAGS=$ac_save_cflags
          ax_add_fortify_3_failed=1
        ])
    if test -n "$ax_add_fortify_3_failed"
    then
    AC_MSG_CHECKING([whether to add -D_FORTIFY_SOURCE=2 to CPPFLAGS])
    AC_LINK_IFELSE([
        AC_LANG_PROGRAM([],
            [[
                #ifndef _FORTIFY_SOURCE
                    return 0;
                #else
                    _FORTIFY_SOURCE_already_defined;
                #endif
            ]]
        )],
        AC_LINK_IFELSE([
            AC_LANG_SOURCE([[
                #define _FORTIFY_SOURCE 2
                #include <string.h>
                int main(void) {
                    char *s = " ";
                    strcpy(s, "x");
                    return strlen(s)-1;
                }
              ]]
            )],
            [
              AC_MSG_RESULT([yes])
              CFLAGS=$ac_save_cflags
              CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"
            ], [
              AC_MSG_RESULT([no])
              CFLAGS=$ac_save_cflags
            ],
        ),
        [
          AC_MSG_RESULT([no])
          CFLAGS=$ac_save_cflags
        ])
    fi
])
