
EXTRA_DIST = \
	cmptest.h \
	windows/windows-quirks.h \
	auth.expected \
	auth2.expected \
	auth3.expected \
	auth5.expected \
	box.expected \
	box2.expected \
	box7.expected \
	box8.expected \
	core1.expected \
	core2.expected \
	core3.expected \
	core4.expected \
	core5.expected \
	core6.expected \
	hash.expected \
	hash2.expected \
	hash3.expected \
	onetimeauth.expected \
	onetimeauth2.expected \
	onetimeauth7.expected \
	scalarmult.expected \
	scalarmult2.expected \
	scalarmult5.expected \
	scalarmult6.expected \
	secretbox.expected \
	secretbox2.expected \
	secretbox7.expected \
	secretbox8.expected \
	stream.expected \
	stream2.expected \
	stream3.expected \
	stream4.expected

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/libsodium/include \
	-I$(top_srcdir)/src/libsodium/include/sodium

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
	secretbox \
	secretbox2 \
	secretbox7 \
	secretbox8 \
	stream \
	stream2 \
	stream3 \
	stream4

check_PROGRAMS = $(TESTS_TARGETS)

TESTS = $(TESTS_TARGETS)

TESTS_LDADD = $(top_srcdir)/src/libsodium/libsodium.la

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

secretbox_SOURCE          = cmptest.h secretbox.c
secretbox_LDADD           = $(TESTS_LDADD)

secretbox2_SOURCE         = cmptest.h secretbox2.c
secretbox2_LDADD          = $(TESTS_LDADD)

secretbox7_SOURCE         = cmptest.h secretbox7.c
secretbox7_LDADD          = $(TESTS_LDADD)

secretbox8_SOURCE         = cmptest.h secretbox8.c
secretbox8_LDADD          = $(TESTS_LDADD)

stream_SOURCE             = cmptest.h stream.c
stream_LDADD              = $(TESTS_LDADD)

stream2_SOURCE            = cmptest.h stream2.c
stream2_LDADD             = $(TESTS_LDADD)

stream3_SOURCE            = cmptest.h stream3.c
stream3_LDADD             = $(TESTS_LDADD)

stream4_SOURCE            = cmptest.h stream4.c
stream4_LDADD             = $(TESTS_LDADD)

verify: check
