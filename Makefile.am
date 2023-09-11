ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	autogen.sh \
	LICENSE \
	README.markdown \
	THANKS

SUBDIRS = \
	builds \
	contrib \
	dist-build \
	src \
	test

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc

DISTCLEANFILES = $(pkgconfig_DATA)

