#! /bin/sh

if [ -z "$NACL_SDK_ROOT" ]; then
  echo "The following variable needs to be set:
  \$NACL_SDK_ROOT=$NACL_SDK_ROOT" >&2
  exit 1
fi

if [ -z "$PNACL_FINALIZE" -o -z "$PNACL_TRANSLATE" ]; then
  exe="$1"
else
  exe="$1.nexe"
  if [ ! -f "$exe" ]; then
    $PNACL_FINALIZE "$1" -o "$1.final"
    $PNACL_TRANSLATE -arch $(uname -m) "$1.final" -o "$exe"
  fi
fi

which python >/dev/null 2>&1 || \
  { echo >&2 "Python not found. Aborting."; exit 1; }

SEL_LDR=$(find "$NACL_SDK_ROOT" -name sel_ldr.py | head -n 1)
if [ -z "$SEL_LDR" ]; then
  echo "Couldn't find sel_ldr.py under $NACL_SDK_ROOT" >&2
  exit 1
fi

exec python "$SEL_LDR" "$exe"
