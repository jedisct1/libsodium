#! /bin/sh

unset LDFLAGS
unset CFLAGS

if command -v wasm-opt >/dev/null; then
  wasm-opt -O4 -o "${1}.tmp" "$1" && mv -f "${1}.tmp" "$1"
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmedge" ]; then
  if command -v wasmedgec >/dev/null && command -v wasmedge >/dev/null; then
    wasmedgec "$1" "${1}.so" >/dev/null &&
      wasmedge --dir=.:. "${1}.so" &&
      rm -f "${1}.so" &&
      exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmer" ]; then
  if command -v wasmer >/dev/null; then
    wasmer run "$1" "--${WASMER_BACKEND:-cranelift}" --dir=. && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmtime" ]; then
  if command -v wasmtime >/dev/null; then
    wasmtime run --dir=. "$1" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "bun" ]; then
  if echo | bun help >/dev/null 2>&1; then
    {
      echo "import fs from 'fs'; import { init, WASI } from '@wasmer/wasi';"
      echo "await init();"
      echo "const wasi = new WASI({args: process.argv, env: process.env, preopens: {'.':'/'}});"
      echo "await (async function() {"
      echo "  const wasm = await WebAssembly.compile(fs.readFileSync('${1}'));"
      echo "  await wasi.instantiate(wasm, {});"
      echo "  wasi.start();"
      echo "  console.log(wasi.getStdoutString());"
      echo "})().catch(e => { console.error(e); process.exit(1); });"
    } >"${1}.mjs"
    bun run "${1}.mjs" 2>/tmp/err &&
      rm -f "${1}.mjs" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "node" ]; then
  if echo | node --experimental-wasi-unstable-preview1 >/dev/null 2>&1; then
    {
      echo "import fs from 'fs'; import { WASI } from 'wasi';"
      echo "const wasi = new WASI({args: process.argv, env: process.env, preopens: {'.':'.'}});"
      echo "const importObject = { wasi_snapshot_preview1: wasi.wasiImport };"
      echo "await (async function() {"
      echo "  const wasm = await WebAssembly.compile(fs.readFileSync('${1}'));"
      echo "  const instance = await WebAssembly.instantiate(wasm, importObject);"
      echo "  wasi.start(instance);"
      echo "})().catch(e => { console.error(e); process.exit(1); });"
    } >"${1}.mjs"
    node --experimental-wasi-unstable-preview1 "${1}.mjs" 2>/tmp/err &&
      rm -f "${1}.mjs" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasm3" ]; then
  if command -v wasm3 >/dev/null; then
    wasm3 "$1" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "iwasm" ]; then
  if command -v iwasm >/dev/null; then
  if iwasm | grep -qi wasi >/dev/null 2>&1; then
    if wamrc --version; then
      wamrc -o "${1}.o" "$1" >/dev/null &&
        iwasm --dir=. "${1}.o" && exit 0
    else
      iwasm --dir=. "$1" && exit 0
    fi
  fi
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wazero" ]; then
  if command -v wazero >/dev/null; then
    wazero run -mount .:/ "$1" && exit 0
  fi
fi

if [ -z "$WASI_RUNTIME" ] || [ "$WASI_RUNTIME" = "wasmer-js" ]; then
  if command -v wasmer-js >/dev/null; then
    wasmer-js run "$1" --dir=. && exit 0
  fi
fi

echo "WebAssembly runtime failed" >&2
exit 1
