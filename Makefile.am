ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	autogen.sh \
	LICENSE \
	README.markdown \
	THANKS

SUBDIRS = \
	src \
	test

if HAVE_PKG_CONFIG
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc
endif
