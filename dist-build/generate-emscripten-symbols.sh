#! /bin/sh

if [ "x$1" = "x--sumo" ]; then
  SUMO=yes
fi

{
while read symbol standard sumo; do
  found="$standard"
  if [ "x$SUMO" != "x" ]; then
    found="$sumo"
  fi
  if [ "$found" = "1" ]; then
    eval "defined_${symbol}=yes"
  else
    eval "defined_${symbol}=no"
  fi
done < emscripten-symbols.def

nm /usr/local/lib/libsodium.18.dylib | \
fgrep ' T _' | \
cut -d' ' -f3 | {
    while read symbol; do
      eval "found=\$defined_${symbol}"
      if [ "$found" = "yes" ]; then
        echo "$symbol"
      elif [ "$found" != "no" ]; then
        echo >&2
        echo "*** [$symbol] was not expected ***" >&2
        echo >&2
        exit 1
      fi
    done
  }
} | \
sort | \
{
out=''
while read symbol ; do
  out="${out},\"${symbol}\""
done
echo $out
}
