#! /bin/sh

if glibtoolize --version >/dev/null 2>&1; then
  LIBTOOLIZE='glibtoolize'
else
  LIBTOOLIZE='libtoolize'
fi

command -v command >/dev/null 2>&1 || {
  echo "command is required, but wasn't found on this system"
  exit 1
}

command -v $LIBTOOLIZE >/dev/null 2>&1 || {
  echo "libtool is required, but wasn't found on this system"
  exit 1
}

command -v autoconf >/dev/null 2>&1 || {
  echo "autoconf is required, but wasn't found on this system"
  exit 1
}

command -v automake >/dev/null 2>&1 || {
  echo "automake is required, but wasn't found on this system"
  exit 1
}

if autoreconf --version >/dev/null 2>&1; then
  autoreconf -ivf
else
  $LIBTOOLIZE &&
    aclocal &&
    automake --add-missing --force-missing --include-deps &&
    autoconf
fi

[ -z "$DO_NOT_UPDATE_CONFIG_SCRIPTS" ] &&
  command -v curl >/dev/null 2>&1 && {
  echo "Downloading config.guess and config.sub..."

  curl -sL -o config.guess \
    'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD' &&
    mv -f config.guess build-aux/config.guess

  curl -sL -o config.sub \
    'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD' &&
    mv -f config.sub build-aux/config.sub

  echo "Done."
}

rm -f config.guess config.sub
