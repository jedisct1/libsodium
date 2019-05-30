#! /bin/sh

MAX_MEMORY_TESTS="67108864"

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmtime" ]; then
  if command -v wasmtime >/dev/null; then
    wasmtime -o --dir=. "$1" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "lucet" ]; then
  if command -v lucetc-wasi >/dev/null && command -v lucet-wasi >/dev/null; then
    lucetc-wasi \
      --reserved-size "${MAX_MEMORY_TESTS}" \
      -o "${1}.so" --opt-level fast "$1" &&
      lucet-wasi --dir=.:. --max-heap-size "${MAX_MEMORY_TESTS}" "${1}.so" &&
      rm -f "${1}.so" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmer" ]; then
  if command -v wasmer >/dev/null; then
    wasmer run "$1" --backend "${WASMER_BACKEND:-cranelift}" --dir=. && exit 0
  fi
fi

echo "WebAssembly runtime failed" >&2
exit 1
