#! /bin/sh

if [ -z "$NACL_SDK_ROOT" -o -z "$PNACL_TRANSLATE" -o -z "$PNACL_FINALIZE" ]
then
	echo "One or more variables need to be set:
	\$NACL_SDK_ROOT=$NACL_SDK_ROOT
	\$PNACL_TRANSLATE=$PNACL_TRANSLATE
	\$PNACL_FINALIZE=$PNACL_FINALIZE"
	exit 1
fi

if [ ! -f "$1.nexe" ]
then
	$PNACL_FINALIZE "$1" -o "$1.final"
	$PNACL_TRANSLATE -arch $(uname -m) "$1.final" -o "$1.nexe"
fi

command -v python >/dev/null 2>&1 || { echo >&2 "I require python but it's not installed. Aborting."; exit 1; }
SEL_LDR=$(find "$NACL_SDK_ROOT" -name sel_ldr.py | head -n 1)
if [ -z "$SEL_LDR" ]
then
	echo "Couldn't find a sel_ldr.py under $NACL_SDK_ROOT"
	exit 1
fi

exec python "$SEL_LDR" "$1.nexe"
