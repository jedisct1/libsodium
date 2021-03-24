#! /bin/sh

set -e

lib="$(
  /usr/bin/nm /usr/local/lib/libsodium.23.dylib | \
  fgrep ' T _' | \
  cut -d' ' -f3
)"

symbols() {
  {
    SUMO="$1"
    while read symbol standard sumo; do
      found="$standard"
      if [ "x$SUMO" = "xsumo" ]; then
        found="$sumo"
      fi
      if [ "$found" = "1" ]; then
        if [ $(echo "$lib" | fgrep "$symbol" | head -n 1) = "$symbol" ]; then
          eval "defined_${symbol}=yes"
        else
          echo >&2
          echo "*** [$symbol] was not expected ***" >&2
          echo >&2
          exit 1
        fi
      fi
    done <emscripten-symbols.def

    echo "$lib" | {
      while read symbol; do
        eval "found=\$defined_${symbol}"
        if [ "$found" = "yes" ]; then
          echo "$symbol"
        fi
      done
    }
  } |
    sort |
    {
      out='"_malloc","_free"'
      while read symbol; do
        if [ ! -z "$out" ]; then
          out="${out},"
        fi
        out="${out}\"${symbol}\""
      done
      echo "[${out}]"
    }
}

out=$(symbols standard)
sed s/EXPORTED_FUNCTIONS_STANDARD=\'.*\'/EXPORTED_FUNCTIONS_STANDARD=\'${out}\'/ <emscripten.sh >emscripten.sh.tmp &&
  mv -f emscripten.sh.tmp emscripten.sh

out=$(symbols sumo)
sed s/EXPORTED_FUNCTIONS_SUMO=\'.*\'/EXPORTED_FUNCTIONS_SUMO=\'${out}\'/ <emscripten.sh >emscripten.sh.tmp &&
  mv -f emscripten.sh.tmp emscripten.sh

chmod +x emscripten.sh
