#!/bin/sh -e

mkdir include

(
  echo x86
  echo unknown
) | (
  while read n
  do
    okabi | (
      while read abi
      do
        okc-$abi | (
          while read c
          do
            echo "=== `date` === Trying $n.c with $c..." >&2
            rm -f cpuid.c
	    cp $n.c cpuid.c || continue
            $c -o cpuid cpuid.c || continue
            $c -o cbytes cbytes.c || continue
	    ./cpuid > cpuid.out || continue
	    echo 'static const char cpuid[] = {' > cpuid.h || continue
	    ./cbytes < cpuid.out >> cpuid.h || continue
	    echo '} ;' >> cpuid.h || continue
	    cp cpuid.h include/cpuid.h || continue
	    cat cpuid.out
            exit 0
          done
          exit 111
        ) && exit 0
      done
      exit 111
    ) && exit 0
  done
  exit 111
)
