
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

auth_SOURCE               = auth.c
auth_LDADD                = $(TESTS_LDADD)

auth2_SOURCE              = auth2.c
auth2_LDADD               = $(TESTS_LDADD)

auth3_SOURCE              = auth3.c
auth3_LDADD               = $(TESTS_LDADD)

auth5_SOURCE              = auth5.c windows/windows-quirks.h
auth5_LDADD               = $(TESTS_LDADD)

box_SOURCE                = box.c
box_LDADD                 = $(TESTS_LDADD)

box2_SOURCE               = box2.c
box2_LDADD                = $(TESTS_LDADD)

box7_SOURCE               = box7.c
box7_LDADD                = $(TESTS_LDADD)

box8_SOURCE               = box8.c
box8_LDADD                = $(TESTS_LDADD)

core1_SOURCE              = core1.c
core1_LDADD               = $(TESTS_LDADD)

core2_SOURCE              = core2.c
core2_LDADD               = $(TESTS_LDADD)

core3_SOURCE              = core3.c
core3_LDADD               = $(TESTS_LDADD)

core4_SOURCE              = core4.c
core4_LDADD               = $(TESTS_LDADD)

core5_SOURCE              = core5.c
core5_LDADD               = $(TESTS_LDADD)

core6_SOURCE              = core6.c
core6_LDADD               = $(TESTS_LDADD)

hash_SOURCE               = hash.c
hash_LDADD                = $(TESTS_LDADD)

hash3_SOURCE              = hash3.c
hash3_LDADD               = $(TESTS_LDADD)

onetimeauth_SOURCE        = onetimeauth.c
onetimeauth_LDADD         = $(TESTS_LDADD)

onetimeauth2_SOURCE       = onetimeauth2.c
onetimeauth2_LDADD        = $(TESTS_LDADD)

onetimeauth7_SOURCE       = onetimeauth7.c
onetimeauth7_LDADD        = $(TESTS_LDADD)

randombytes_SOURCE        = randombytes.c
randombytes_LDADD         = $(TESTS_LDADD)

scalarmult_SOURCE         = scalarmult.c
scalarmult_LDADD          = $(TESTS_LDADD)

scalarmult2_SOURCE        = scalarmult2.c
scalarmult2_LDADD         = $(TESTS_LDADD)

scalarmult5_SOURCE        = scalarmult5.c
scalarmult5_LDADD         = $(TESTS_LDADD)

scalarmult6_SOURCE        = scalarmult6.c
scalarmult6_LDADD         = $(TESTS_LDADD)

secretbox_SOURCE          = secretbox.c
secretbox_LDADD           = $(TESTS_LDADD)

secretbox2_SOURCE         = secretbox2.c
secretbox2_LDADD          = $(TESTS_LDADD)

secretbox7_SOURCE         = secretbox7.c
secretbox7_LDADD          = $(TESTS_LDADD)

secretbox8_SOURCE         = secretbox8.c
secretbox8_LDADD          = $(TESTS_LDADD)

stream_SOURCE             = stream.c
stream_LDADD              = $(TESTS_LDADD)

stream2_SOURCE            = stream2.c
stream2_LDADD             = $(TESTS_LDADD)

stream3_SOURCE            = stream3.c
stream3_LDADD             = $(TESTS_LDADD)

stream4_SOURCE            = stream4.c
stream4_LDADD             = $(TESTS_LDADD)

verify: check
