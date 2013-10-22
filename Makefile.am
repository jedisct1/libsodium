ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	autogen.sh \
	libsodium.sln \
	libsodium.vcxproj \
	libsodium.vcxproj.filters \
	LICENSE \
	README.markdown \
	THANKS

SUBDIRS = \
	msvc-scripts \
	src \
	test

if HAVE_PKG_CONFIG
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc
endif
