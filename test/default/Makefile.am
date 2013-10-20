
EXTRA_DIST = \
	cmptest.h \
	wintest.bat \
	auth.exp \
	auth2.exp \
	auth3.exp \
	auth5.exp \
	box.exp \
	box2.exp \
	box7.exp \
	box8.exp \
	core1.exp \
	core2.exp \
	core3.exp \
	core4.exp \
	core5.exp \
	core6.exp \
	generichash.exp \
	generichash2.exp \
	hash.exp \
	hash2.exp \
	hash3.exp \
	onetimeauth.exp \
	onetimeauth2.exp \
	onetimeauth7.exp \
	scalarmult.exp \
	scalarmult2.exp \
	scalarmult5.exp \
	scalarmult6.exp \
	scalarmult7.exp \
	scalarmult8.exp \
	secretbox.exp \
	secretbox2.exp \
	secretbox7.exp \
	secretbox8.exp \
	shorthash.exp \
	sodium_core.exp \
	sodium_utils.exp \
	sodium_version.exp \
	stream.exp \
	stream2.exp \
	stream3.exp \
	stream4.exp \
	stream5.exp \
	stream6.exp

DISTCLEANFILES = \
	auth.res \
	auth2.res \
	auth3.res \
	auth5.res \
	box.res \
	box2.res \
	box7.res \
	box8.res \
	core1.res \
	core2.res \
	core3.res \
	core4.res \
	core5.res \
	core6.res \
	generichash.res \
	generichash2.res \
	hash.res \
	hash2.res \
	hash3.res \
	onetimeauth.res \
	onetimeauth2.res \
	onetimeauth7.res \
	scalarmult.res \
	scalarmult2.res \
	scalarmult5.res \
	scalarmult6.res \
	scalarmult7.res \
	scalarmult8.res \
	secretbox.res \
	secretbox2.res \
	secretbox7.res \
	secretbox8.res \
	shorthash.res \
	sodium_core.res \
	sodium_utils.res \
	sodium_version.res \
	stream.res \
	stream2.res \
	stream3.res \
	stream4.res \
	stream5.res \
	stream6.res

AM_CPPFLAGS = \
	-DTEST_SRCDIR=\"@srcdir@\" \
	-I$(top_srcdir)/src/libsodium/include \
	-I$(top_srcdir)/src/libsodium/include/sodium \
	-I$(top_builddir)/src/libsodium/include \
	-I$(top_builddir)/src/libsodium/include/sodium \
	-I$(top_srcdir)/test/quirks

TESTS_TARGETS = \
	auth \
	auth2 \
	auth3 \
	auth5 \
	box \
	box2 \
	box7 \
	box8 \
	core1 \
	core2 \
	core3 \
	core4 \
	core5 \
	core6 \
	generichash \
	generichash2 \
	hash \
	hash3 \
	onetimeauth \
	onetimeauth2 \
	onetimeauth7 \
	randombytes \
	scalarmult \
	scalarmult2 \
	scalarmult5 \
	scalarmult6 \
	scalarmult7 \
	scalarmult8 \
	secretbox \
	secretbox2 \
	secretbox7 \
	secretbox8 \
	shorthash \
	sodium_core \
	sodium_utils \
	sodium_version \
	stream \
	stream2 \
	stream3 \
	stream4 \
	stream5 \
	stream6

check_PROGRAMS = $(TESTS_TARGETS)

TESTS = $(TESTS_TARGETS)

TESTS_LDADD = \
	${top_builddir}/src/libsodium/libsodium.la

auth_SOURCE               = cmptest.h auth.c
auth_LDADD                = $(TESTS_LDADD)

auth2_SOURCE              = cmptest.h auth2.c
auth2_LDADD               = $(TESTS_LDADD)

auth3_SOURCE              = cmptest.h auth3.c
auth3_LDADD               = $(TESTS_LDADD)

auth5_SOURCE              = cmptest.h auth5.c windows/windows-quirks.h
auth5_LDADD               = $(TESTS_LDADD)

box_SOURCE                = cmptest.h box.c
box_LDADD                 = $(TESTS_LDADD)

box2_SOURCE               = cmptest.h box2.c
box2_LDADD                = $(TESTS_LDADD)

box7_SOURCE               = cmptest.h box7.c
box7_LDADD                = $(TESTS_LDADD)

box8_SOURCE               = cmptest.h box8.c
box8_LDADD                = $(TESTS_LDADD)

core1_SOURCE              = cmptest.h core1.c
core1_LDADD               = $(TESTS_LDADD)

core2_SOURCE              = cmptest.h core2.c
core2_LDADD               = $(TESTS_LDADD)

core3_SOURCE              = cmptest.h core3.c
core3_LDADD               = $(TESTS_LDADD)

core4_SOURCE              = cmptest.h core4.c
core4_LDADD               = $(TESTS_LDADD)

core5_SOURCE              = cmptest.h core5.c
core5_LDADD               = $(TESTS_LDADD)

core6_SOURCE              = cmptest.h core6.c
core6_LDADD               = $(TESTS_LDADD)

generichash_SOURCE        = cmptest.h generichash.c
generichash_LDADD         = $(TESTS_LDADD)

generichash2_SOURCE       = cmptest.h generichash2.c
generichash2_LDADD        = $(TESTS_LDADD)

hash_SOURCE               = cmptest.h hash.c
hash_LDADD                = $(TESTS_LDADD)

hash3_SOURCE              = cmptest.h hash3.c
hash3_LDADD               = $(TESTS_LDADD)

onetimeauth_SOURCE        = cmptest.h onetimeauth.c
onetimeauth_LDADD         = $(TESTS_LDADD)

onetimeauth2_SOURCE       = cmptest.h onetimeauth2.c
onetimeauth2_LDADD        = $(TESTS_LDADD)

onetimeauth7_SOURCE       = cmptest.h onetimeauth7.c
onetimeauth7_LDADD        = $(TESTS_LDADD)

randombytes_SOURCE        = randombytes.c
randombytes_LDADD         = $(TESTS_LDADD)

scalarmult_SOURCE         = cmptest.h scalarmult.c
scalarmult_LDADD          = $(TESTS_LDADD)

scalarmult2_SOURCE        = cmptest.h scalarmult2.c
scalarmult2_LDADD         = $(TESTS_LDADD)

scalarmult5_SOURCE        = cmptest.h scalarmult5.c
scalarmult5_LDADD         = $(TESTS_LDADD)

scalarmult6_SOURCE        = cmptest.h scalarmult6.c
scalarmult6_LDADD         = $(TESTS_LDADD)

scalarmult7_SOURCE        = cmptest.h scalarmult7.c
scalarmult7_LDADD         = $(TESTS_LDADD)

scalarmult8_SOURCE        = cmptest.h scalarmult8.c
scalarmult8_LDADD         = $(TESTS_LDADD)

secretbox_SOURCE          = cmptest.h secretbox.c
secretbox_LDADD           = $(TESTS_LDADD)

secretbox2_SOURCE         = cmptest.h secretbox2.c
secretbox2_LDADD          = $(TESTS_LDADD)

secretbox7_SOURCE         = cmptest.h secretbox7.c
secretbox7_LDADD          = $(TESTS_LDADD)

secretbox8_SOURCE         = cmptest.h secretbox8.c
secretbox8_LDADD          = $(TESTS_LDADD)

shorthash_SOURCE          = cmptest.h shorthash.c
shorthash_LDADD           = $(TESTS_LDADD)

sodium_core_SOURCE        = cmptest.h sodium_core.c
sodium_core_LDADD         = $(TESTS_LDADD)

sodium_utils_SOURCE       = cmptest.h sodium_utils.c
sodium_utils_LDADD        = $(TESTS_LDADD)

sodium_version_SOURCE     = cmptest.h sodium_version.c
sodium_version_LDADD      = $(TESTS_LDADD)

stream_SOURCE             = cmptest.h stream.c
stream_LDADD              = $(TESTS_LDADD)

stream2_SOURCE            = cmptest.h stream2.c
stream2_LDADD             = $(TESTS_LDADD)

stream3_SOURCE            = cmptest.h stream3.c
stream3_LDADD             = $(TESTS_LDADD)

stream4_SOURCE            = cmptest.h stream4.c
stream4_LDADD             = $(TESTS_LDADD)

stream5_SOURCE            = cmptest.h stream5.c
stream5_LDADD             = $(TESTS_LDADD)

stream6_SOURCE            = cmptest.h stream6.c
stream6_LDADD             = $(TESTS_LDADD)

verify: check
