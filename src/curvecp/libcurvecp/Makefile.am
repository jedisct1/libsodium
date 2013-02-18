
noinst_LTLIBRARIES = \
	libcurvecp.la

libcurvecp_la_SOURCES = \
	blocking.c \
	blocking.h \
	byte.h \
	byte_copy.c \
	byte_isequal.c \
	byte_zero.c \
	crypto_block.c \
	crypto_block.h \
	die.c \
	die.h \
	e.c \
	e.h \
	hexparse.c \
	hexparse.h \
	load.c \
	load.h \
	nameparse.c \
	nameparse.h \
	nanoseconds.c \
	nanoseconds.h \
	open.h \
	open_cwd.c \
	open_lock.c \
	open_pipe.c \
	open_read.c \
	open_write.c \
	portparse.c \
	portparse.h \
	randommod.c \
	randommod.h \
	safenonce.c \
	safenonce.h \
	savesync.c \
	savesync.h \
	socket.h \
	socket_bind.c \
	socket_recv.c \
	socket_send.c \
	socket_udp.c \
	uint16_pack.c \
	uint16_pack.h \
	uint16_unpack.c \
	uint16_unpack.h \
	uint32_pack.c \
	uint32_pack.h \
	uint32_unpack.c \
	uint32_unpack.h \
	uint64_pack.c \
	uint64_pack.h \
	uint64_unpack.c \
	uint64_unpack.h \
	writeall.c \
	writeall.h

libcurvecp_la_LIBADD = \
	$(top_srcdir)/src/libsodium/libsodium.la

libcurvecp_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	-no-undefined

libcurvecp_la_CPPFLAGS = \
	$(LTDLINCL) \
	-I$(top_srcdir)/src/libsodium/include/sodium
