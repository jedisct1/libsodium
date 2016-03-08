# ===========================================================================
#     http://www.gnu.org/software/autoconf-archive/ax_valgrind_check.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_VALGRIND_CHECK()
#
# DESCRIPTION
#
#   Checks whether Valgrind is present and, if so, allows running `make
#   check` under a variety of Valgrind tools to check for memory and
#   threading errors.
#
#   Defines VALGRIND_CHECK_RULES which should be substituted in your
#   Makefile; and $enable_valgrind which can be used in subsequent configure
#   output. VALGRIND_ENABLED is defined and substituted, and corresponds to
#   the value of the --enable-valgrind option, which defaults to being
#   enabled if Valgrind is installed and disabled otherwise.
#
#   If unit tests are written using a shell script and automake's
#   LOG_COMPILER system, the $(VALGRIND) variable can be used within the
#   shell scripts to enable Valgrind, as described here:
#
#     https://www.gnu.org/software/gnulib/manual/html_node/Running-self_002dtests-under-valgrind.html
#
#   Usage example:
#
#   configure.ac:
#
#     AX_VALGRIND_CHECK
#
#   Makefile.am:
#
#     @VALGRIND_CHECK_RULES@
#     VALGRIND_SUPPRESSIONS_FILES = my-project.supp
#     EXTRA_DIST = my-project.supp
#
#   This results in a "check-valgrind" rule being added to any Makefile.am
#   which includes "@VALGRIND_CHECK_RULES@" (assuming the module has been
#   configured with --enable-valgrind). Running `make check-valgrind` in
#   that directory will run the module's test suite (`make check`) once for
#   each of the available Valgrind tools (out of memcheck, helgrind, drd and
#   sgcheck), and will output results to test-suite-$toolname.log for each.
#   The target will succeed if there are zero errors and fail otherwise.
#
#   The macro supports running with and without libtool.
#
# LICENSE
#
#   Copyright (c) 2014, 2015, 2016 Philip Withnall <philip.withnall@collabora.co.uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_VALGRIND_CHECK],[
	dnl Check for --enable-valgrind
	AC_ARG_ENABLE([valgrind],
	              [AS_HELP_STRING([--enable-valgrind], [Whether to enable Valgrind on the unit tests (requires GNU make)])],
	              [enable_valgrind=$enableval],[enable_valgrind=no])

	AS_IF([test "$enable_valgrind" != "no"],[
		# Check for Valgrind.
		AC_CHECK_PROG([VALGRIND],[valgrind],[valgrind])
		AS_IF([test "$VALGRIND" = ""],[
			AS_IF([test "$enable_valgrind" = "yes"],[
				AC_MSG_ERROR([Could not find valgrind; either install it or reconfigure with --disable-valgrind])
			],[
				enable_valgrind=no
			])
		],[
			enable_valgrind=yes
		])
	])

	AM_CONDITIONAL([VALGRIND_ENABLED],[test "$enable_valgrind" = "yes"])
	AC_SUBST([VALGRIND_ENABLED],[$enable_valgrind])

	# Check for Valgrind tools we care about.
	m4_define([valgrind_tool_list],[[memcheck], [helgrind], [drd], [exp-sgcheck]])

	AS_IF([test "$VALGRIND" != ""],[
		m4_foreach([vgtool],[valgrind_tool_list],[
			m4_define([vgtooln],AS_TR_SH(vgtool))
			m4_define([ax_cv_var],[ax_cv_valgrind_tool_]vgtooln)
			AC_CACHE_CHECK([for Valgrind tool ]vgtool,ax_cv_var,[
				ax_cv_var=
				AS_IF([`$VALGRIND --tool=vgtool --help >/dev/null 2>&1`],[
					ax_cv_var="vgtool"
				])
			])

			AC_SUBST([VALGRIND_HAVE_TOOL_]vgtooln,[$ax_cv_var])
		])
	])

[VALGRIND_CHECK_RULES='
# Valgrind check
#
# Optional:
#  - VALGRIND_SUPPRESSIONS_FILES: Space-separated list of Valgrind suppressions
#    files to load. (Default: empty)
#  - VALGRIND_FLAGS: General flags to pass to all Valgrind tools.
#    (Default: --num-callers=30)
#  - VALGRIND_$toolname_FLAGS: Flags to pass to Valgrind $toolname (one of:
#    memcheck, helgrind, drd, sgcheck). (Default: various)

# Optional variables
VALGRIND_SUPPRESSIONS ?= $(addprefix --suppressions=,$(VALGRIND_SUPPRESSIONS_FILES))
VALGRIND_FLAGS ?= --num-callers=30
VALGRIND_memcheck_FLAGS ?= --leak-check=full --show-reachable=no
VALGRIND_helgrind_FLAGS ?= --history-level=approx
VALGRIND_drd_FLAGS ?=
VALGRIND_sgcheck_FLAGS ?=

# Internal use
valgrind_tools = memcheck helgrind drd sgcheck
valgrind_log_files = $(addprefix test-suite-,$(addsuffix .log,$(valgrind_tools)))

valgrind_memcheck_flags = --tool=memcheck $(VALGRIND_memcheck_FLAGS)
valgrind_helgrind_flags = --tool=helgrind $(VALGRIND_helgrind_FLAGS)
valgrind_drd_flags = --tool=drd $(VALGRIND_drd_FLAGS)
valgrind_sgcheck_flags = --tool=exp-sgcheck $(VALGRIND_sgcheck_FLAGS)

valgrind_quiet = $(valgrind_quiet_$(V))
valgrind_quiet_ = $(valgrind_quiet_$(AM_DEFAULT_VERBOSITY))
valgrind_quiet_0 = --quiet

# Support running with and without libtool.
ifneq ($(LIBTOOL),)
valgrind_lt = $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=execute
else
valgrind_lt =
endif

# Use recursive makes in order to ignore errors during check
check-valgrind:
ifeq ($(VALGRIND_ENABLED),yes)
	-$(foreach tool,$(valgrind_tools), \
		$(if $(VALGRIND_HAVE_TOOL_$(tool))$(VALGRIND_HAVE_TOOL_exp_$(tool)), \
			$(MAKE) $(AM_MAKEFLAGS) -k check-valgrind-tool VALGRIND_TOOL=$(tool); \
		) \
	)
else
	@echo "Need to reconfigure with --enable-valgrind"
endif

# Valgrind running
VALGRIND_TESTS_ENVIRONMENT = \
	$(TESTS_ENVIRONMENT) \
	env VALGRIND=$(VALGRIND) \
	G_SLICE=always-malloc,debug-blocks \
	G_DEBUG=fatal-warnings,fatal-criticals,gc-friendly

VALGRIND_LOG_COMPILER = \
	$(valgrind_lt) \
	$(VALGRIND) $(VALGRIND_SUPPRESSIONS) --error-exitcode=1 $(VALGRIND_FLAGS)

check-valgrind-tool:
ifeq ($(VALGRIND_ENABLED),yes)
	$(MAKE) check-TESTS \
		TESTS_ENVIRONMENT="$(VALGRIND_TESTS_ENVIRONMENT)" \
		LOG_COMPILER="$(VALGRIND_LOG_COMPILER)" \
		LOG_FLAGS="$(valgrind_$(VALGRIND_TOOL)_flags)" \
		TEST_SUITE_LOG=test-suite-$(VALGRIND_TOOL).log
else
	@echo "Need to reconfigure with --enable-valgrind"
endif

A''M_DISTCHECK_CONFIGURE_FLAGS ?=
A''M_DISTCHECK_CONFIGURE_FLAGS += --disable-valgrind

MOSTLYCLEANFILES ?=
MOSTLYCLEANFILES += $(valgrind_log_files)

.PHONY: check-valgrind check-valgrind-tool
']

	AS_IF([test "$enable_valgrind" != "yes"],[
VALGRIND_CHECK_RULES='
check-valgrind:
	@echo "Need to use GNU make and reconfigure with --enable-valgrind"'
])
	AC_SUBST([VALGRIND_CHECK_RULES])
	m4_ifdef([_AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE([VALGRIND_CHECK_RULES])])
])
