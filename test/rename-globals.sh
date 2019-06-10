#! /bin/sh

LIBSODIUM_DIR="$(mktemp -d)"
LIBSODIUM_PATH="${LIBSODIUM_DIR}/lib/libsodium.a"
SYMBOLS_DIR="test/symbols"
ALL_HOST_SYMBOLS="${SYMBOLS_DIR}/all-host-symbols.txt"
ALL_HOST_SYMBOLS_NEW="${ALL_HOST_SYMBOLS}.new"
ALL_SYMBOLS="${SYMBOLS_DIR}/all-symbols.txt"
ALL_SYMBOLS_NEW="${ALL_SYMBOLS}.new"
INTERNAL_HOST_SYMBOLS="${SYMBOLS_DIR}/internal-host-symbols.txt"
INTERNAL_HOST_SYMBOLS_NEW="${INTERNAL_HOST_SYMBOLS}.new"
INTERNAL_SYMBOLS="${SYMBOLS_DIR}/internal-symbols.txt"
INTERNAL_SYMBOLS_NEW="${INTERNAL_SYMBOLS}.new"
INTERNAL_SYMBOL_PREFIX="_sodium_"
QUIRKS_H="src/libsodium/include/sodium/private/quirks.h"
QUIRKS_H_NEW="${QUIRKS_H}.new"
CFLAGS_UNOPTIMIZED="-O0 -fno-lto"
MAKE_OPTS="-j$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)"

NM="nm"
NM_OPTS=""

export LC_ALL=C
export LC_COLLATE=C

if [ ! -r "$QUIRKS_H" ]; then
    cd ..
    if [ ! -r "$QUIRKS_H" ]; then
        echo "[$QUIRKS_H] not found" >&2
        exit 1
    fi
fi

echo "Compiling without quirks..."
make distclean 2>/dev/null
env CFLAGS="$CFLAGS_UNOPTIMIZED" CPPFLAGS="-DNO_QUIRKS=1 -DDEV_MODE=1" \
    ./configure --prefix="$LIBSODIUM_DIR" \
    --disable-shared --enable-static --disable-dependency-tracking >/dev/null || exit 1
make clean >/dev/null || exit 1
make $MAKE_OPTS install >/dev/null 2>&1 || exit 1
echo "Done."
echo

mkdir -p "$SYMBOLS_DIR" || exit 1
[ -e "$ALL_SYMBOLS" ] || touch "$ALL_SYMBOLS"
[ -e "$ALL_HOST_SYMBOLS" ] || touch "$ALL_HOST_SYMBOLS"
[ -e "$QUIRKS_H" ] || touch "$QUIRKS_H"
if [ -e "$INTERNAL_SYMBOLS" ]; then
    grep -v "^sodium_" "$ALL_SYMBOLS" |
        grep -v "^crypto_" |
        grep -v "^randombytes" >"$INTERNAL_SYMBOLS"
fi

for opt in --demangle --defined-only -P; do
    "$NM" $NM_OPTS $opt "$LIBSODIUM_PATH" >/dev/null 2>&1 &&
        NM_OPTS="${NM_OPTS} ${opt}"
done
echo "nm: [${NM} ${NM_OPTS}]"

sodium_init_symbol=$("$NM" $NM_OPTS "$LIBSODIUM_PATH" |
    grep "^[^ ]*sodium_init T " | cut -d " " -f1 | cut -d "(" -f1)
if ! echo "$sodium_init_symbol" | grep -q "sodium_init$"; then
    echo "Unable to parse symbols" >&2
    exit 1
fi

SYMBOL_PREFIX=$(echo "$sodium_init_symbol" | sed "s/sodium_init$//")
echo "symbol prefix: [${SYMBOL_PREFIX}]"

"$NM" $NM_OPTS "$LIBSODIUM_PATH" | grep "^${SYMBOL_PREFIX}[^ ]* T " |
    cut -d " " -f1 | cut -d "(" -f1 | sed "s/^${SYMBOL_PREFIX}//" |
    grep -v "^_" | sort -u >"${ALL_HOST_SYMBOLS_NEW}.tmp"

cat "${ALL_HOST_SYMBOLS_NEW}.tmp" "$ALL_HOST_SYMBOLS" | sort -u >"$ALL_HOST_SYMBOLS_NEW" &&
    rm -f "${ALL_HOST_SYMBOLS_NEW}.tmp"
cat "$ALL_HOST_SYMBOLS_NEW" "$ALL_SYMBOLS" | sort -u >"$ALL_SYMBOLS_NEW"

grep -v "^sodium_" "$ALL_HOST_SYMBOLS_NEW" |
    grep -v "^crypto_" |
    grep -v "^randombytes" >"$INTERNAL_HOST_SYMBOLS_NEW"

grep -v "^sodium_" "$ALL_SYMBOLS_NEW" |
    grep -v "^crypto_" |
    grep -v "^randombytes" >"$INTERNAL_SYMBOLS_NEW"

cat >"$QUIRKS_H_NEW" <<EOT
/* This is an automatically generated file */

#ifndef quirks_H
#ifndef NO_QUIRKS

EOT
while read -r fn; do
    prefixed_fn="${INTERNAL_SYMBOL_PREFIX}${fn}"
    if grep "^${prefixed_fn}$" "$ALL_SYMBOLS_NEW"; then
        echo "Name collision: [${fn}] cannot be renamed to existing [${prefixed_fn}]" >&2
        exit 1
    fi
    echo "#define ${fn} ${prefixed_fn}" >>"$QUIRKS_H_NEW"
done <"$INTERNAL_SYMBOLS_NEW"
cat >>"$QUIRKS_H_NEW" <<EOT

#endif
#endif
EOT

if [ -s "$ALL_SYMBOLS" ]; then
    echo
    echo "* Changes to all symbols:"
    diff -u "$ALL_SYMBOLS" "$ALL_SYMBOLS_NEW"
fi

if [ -s "$ALL_HOST_SYMBOLS" ]; then
    echo
    echo "* Changes to host symbols:"
    diff -u "$ALL_HOST_SYMBOLS" "$ALL_HOST_SYMBOLS_NEW"
fi

if [ -s "$INTERNAL_SYMBOLS" ]; then
    echo
    echo "* Changes to internal symbols:"
    diff -u "$INTERNAL_SYMBOLS" "$INTERNAL_SYMBOLS_NEW"
fi

if [ -s "$INTERNAL_HOST_SYMBOLS" ]; then
    echo
    echo "* Changes to internal host symbols:"
    diff -u "$INTERNAL_HOST_SYMBOLS" "$INTERNAL_HOST_SYMBOLS_NEW"
fi

if [ -s "$QUIRKS_H" ]; then
    echo
    echo "* Changes to the quirks header file:"
    diff -u "$QUIRKS_H" "$QUIRKS_H_NEW"
fi

echo

if diff -u "$QUIRKS_H" "$QUIRKS_H_NEW" | grep -v "^+++" | grep -q "^+" >/dev/null; then
    echo "The quirks header file needs an update." >&2
    exit 1
fi
if ! cmp "$QUIRKS_H" "$QUIRKS_H_NEW"; then
    echo "The quirks header file may need an update." >&2
fi

mv -f "$QUIRKS_H" "${QUIRKS_H}.old"
mv -f "$QUIRKS_H_NEW" "$QUIRKS_H"
mv -f "$ALL_HOST_SYMBOLS_NEW" "$ALL_HOST_SYMBOLS"
mv -f "$ALL_SYMBOLS_NEW" "$ALL_SYMBOLS"
mv -f "$INTERNAL_HOST_SYMBOLS_NEW" "$INTERNAL_HOST_SYMBOLS"
mv -f "$INTERNAL_SYMBOLS_NEW" "$INTERNAL_SYMBOLS"

echo "Compiling with quirks..."
env CFLAGS="$CFLAGS_UNOPTIMIZED" CPPFLAGS="-DDEV_MODE=1" \
    ./configure --prefix="$LIBSODIUM_DIR" \
    --disable-shared --enable-static --disable-dependency-tracking >/dev/null || exit 1
make clean >/dev/null || exit 1
make $MAKE_OPTS install >/dev/null 2>&1 || exit 1

"$NM" $NM_OPTS "$LIBSODIUM_PATH" | grep "^${SYMBOL_PREFIX}[^ ]* T " |
    cut -d " " -f1 | cut -d "(" -f1 | sed "s/^${SYMBOL_PREFIX}//" |
    grep -v "^_" | sort -u |
    grep -v "^sodium_" |
    grep -v "^crypto_" |
    grep -v "^randombytes" >"$INTERNAL_HOST_SYMBOLS_NEW"

if [ -s "$INTERNAL_SYMBOLS_NEW" ]; then
    echo "Internal symbols are still present:" >&2
    echo >&2
    cat "$INTERNAL_HOST_SYMBOLS_NEW" >&2
    exit 1
fi

rm -f "$INTERNAL_HOST_SYMBOLS_NEW"

rm -fr "$LIBSODIUM_DIR"

echo "Done."
