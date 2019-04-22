#! /bin/sh

if command -v wasmtime >/dev/null; then
  wasmtime -o --dir=. "$1" && exit 0
fi
if command -v wasmer >/dev/null; then
  wasmer run "$1" --dir=. && exit 0
fi
if command -v lucetc-wasi >/dev/null && command -v lucet-wasi >/dev/null; then
  lucetc-wasi -o "${1}.so" --opt-level best "$1" && lucet-wasi --dir=.:. "$1.so" && exit 0
fi
