
AM_CPPFLAGS = \
	-I. \
	-I$(top_srcdir)/src/libsodium/include

TESTS_TARGETS = \
	test-randombytes

check_PROGRAMS = $(TESTS_TARGETS)

TESTS = $(TESTS_TARGETS)

TESTS_LDADD = $(top_srcdir)/src/libsodium/libsodium.la

test_randombytes_SOURCES = test-randombytes.c
test_randombytes_LDADD   = $(TESTS_LDADD)

verify: check
