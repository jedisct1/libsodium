
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/libsodium/include \
	-I$(top_srcdir)/src/libsodium/include/sodium

TESTS_TARGETS = \
	test-randombytes \
	auth

check_PROGRAMS = $(TESTS_TARGETS)

TESTS = $(TESTS_TARGETS)

TESTS_LDADD = $(top_srcdir)/src/libsodium/libsodium.la

test_randombytes_SOURCES  = test-randombytes.c
test_randombytes_LDADD    = $(TESTS_LDADD)

auth_SOURCES              = auth.c cmptest.h
auth_LDADD                = $(TESTS_LDADD)

verify: check
