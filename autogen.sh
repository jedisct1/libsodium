#! /bin/sh

args=$(getopt bfos "$@")
if [ $? -ne 0 ]; then
  echo "Usage: autogen.sh [-b] [-f] [-o] [-s] [--]"
  echo
  echo ">   -b: do not update the system detection scripts"
  echo ">   -f: force the recreation of all autoconf scripts"
  echo ">   -o: overwrite/downgrade system detection scripts"
  echo ">   -s: setup an environment for developers"
  exit 2
fi

force=false
update_config=true
overwrite_config=false
dev_setup=false

eval set -- "$args"

while [ $# -ne 0 ]; do
  case $1 in
  -b)
    update_config=false
    ;;
  -f)
    force=true
    ;;
  -o)
    overwrite_config=true
    ;;
  -s)
    dev_setup=true
    ;;
  --)
    shift
    break
    ;;
  esac
  shift
done

if [ -s configure ]; then
  if [ "$force" != true ]; then
    echo "autoconf scripts already exist." >&2
    exit 0
  fi
elif [ "$dev_setup" != true ]; then
  echo "A development environment was not created."
  exit 0
fi

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

if [ "$overwrite_config" = false ]; then
  if [ -f build-aux/config.guess ]; then
    mv build-aux/config.guess build-aux/config.guess.stable
  fi
  if [ -f build-aux/config.sub ]; then
    mv build-aux/config.sub build-aux/config.sub.stable
  fi
else
  rm -fr build-aux
fi

$LIBTOOLIZE --copy --install &&
  aclocal &&
  automake --add-missing --copy --force-missing --include-deps &&
  autoconf && echo Done.
if [ "$overwrite_config" = false ]; then
  if [ -f build-aux/config.guess.stable ]; then
    mv build-aux/config.guess.stable build-aux/config.guess
  fi
  if [ -f build-aux/config.sub.stable ]; then
    mv build-aux/config.sub.stable build-aux/config.sub
  fi
fi

[ "$update_config" = true ] && [ -z "$DO_NOT_UPDATE_CONFIG_SCRIPTS" ] &&
  command -v curl >/dev/null 2>&1 && {
  echo "Downloading config.guess and config.sub..."

  for file in config.guess config.sub; do
    if curl -sSL --fail --retry 3 --connect-timeout 10 --max-time 60 \
      -o "$file" "https://git.savannah.gnu.org/cgit/config.git/plain/$file" &&
      chmod +x "$file" &&
      mv -f "$file" "build-aux/$file"; then
      echo "Updated $file."
    else
      echo "Warning: unable to update $file; keeping the existing copy." >&2
    fi
  done
}

rm -f config.guess config.sub
