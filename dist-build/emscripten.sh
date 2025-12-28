#! /bin/sh

export MAKE_FLAGS='-j4'
export EXPORTED_FUNCTIONS_STANDARD='["_malloc","_free"]'
export EXPORTED_FUNCTIONS_SUMO='["_malloc","_free"]'
export EXPORTED_RUNTIME_METHODS='["UTF8ToString","getValue","setValue","HEAPU8"]'
export JS_RESERVED_MEMORY_STANDARD=16MB
export JS_RESERVED_MEMORY_SUMO=48MB
export JS_RESERVED_MEMORY_TESTS=16MB
export WASM_INITIAL_MEMORY=4MB
export LDFLAGS="-s RESERVED_FUNCTION_POINTERS=8"
export LDFLAGS="${LDFLAGS} -s ALLOW_MEMORY_GROWTH=1"
export LDFLAGS="${LDFLAGS} -s SINGLE_FILE=1"
export LDFLAGS="${LDFLAGS} -s ASSERTIONS=0"
export LDFLAGS="${LDFLAGS} -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s ALIASING_FUNCTION_POINTERS=1"
export LDFLAGS="${LDFLAGS} -s DISABLE_EXCEPTION_CATCHING=1"
export LDFLAGS="${LDFLAGS} -s ELIMINATE_DUPLICATE_FUNCTIONS=1"
export LDFLAGS="${LDFLAGS} -s NODEJS_CATCH_EXIT=0"
export LDFLAGS="${LDFLAGS} -s NODEJS_CATCH_REJECTION=0"
export LDFLAGS="${LDFLAGS} -s WASM_BIGINT=0"

echo
if [ "$1" = "--standard" ]; then
  export EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS_STANDARD"
  export LDFLAGS="${LDFLAGS} ${LDFLAGS_DIST}"
  export LDFLAGS_JS="-s TOTAL_MEMORY=${JS_RESERVED_MEMORY_STANDARD}"
  export PREFIX="$(pwd)/libsodium-js"
  export DONE_FILE="$(pwd)/js.done"
  export CONFIG_EXTRA="--enable-minimal"
  export DIST='yes'
  echo "Building a standard distribution in [${PREFIX}]"
elif [ "$1" = "--sumo" ]; then
  export EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS_SUMO"
  export LDFLAGS="${LDFLAGS} ${LDFLAGS_DIST}"
  export LDFLAGS_JS="-s TOTAL_MEMORY=${JS_RESERVED_MEMORY_SUMO}"
  export PREFIX="$(pwd)/libsodium-js-sumo"
  export DONE_FILE="$(pwd)/js-sumo.done"
  export DIST='yes'
  echo "Building a sumo distribution in [${PREFIX}]"
elif [ "$1" = "--browser-tests" ]; then
  export EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS_SUMO"
  export CPPFLAGS="${CPPFLAGS} -s FORCE_FILESYSTEM=1"
  export LDFLAGS="${LDFLAGS}"
  export LDFLAGS_JS="-s TOTAL_MEMORY=${JS_RESERVED_MEMORY_TESTS}"
  export PREFIX="$(pwd)/libsodium-js-tests"
  export DONE_FILE="$(pwd)/js-tests-browser.done"
  export BROWSER_TESTS='yes'
  export DIST='no'
  echo "Building tests for web browsers in [${PREFIX}]"
elif [ "$1" = "--tests" ]; then
  echo "Building for testing"
  export EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS_SUMO"
  export CPPFLAGS="${CPPFLAGS} -s FORCE_FILESYSTEM=1 -DBENCHMARKS -DITERATIONS=10"
  export LDFLAGS="${LDFLAGS}"
  export LDFLAGS_JS="-s TOTAL_MEMORY=${JS_RESERVED_MEMORY_TESTS}"
  export PREFIX="$(pwd)/libsodium-js-tests"
  export DONE_FILE="$(pwd)/js-tests.done"
  export DIST='no'
  echo "Building for testing in [${PREFIX}]"
else
  echo "Usage: $0 <build_type>"
  echo "<build_type> := --standard | --sumo | --browser-tests | --tests"
  echo
  exit 1
fi
export JS_EXPORTS_FLAGS="-s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} -s EXPORTED_RUNTIME_METHODS=${EXPORTED_RUNTIME_METHODS}"

rm -f "$DONE_FILE"

echo

emconfigure ./configure $CONFIG_EXTRA --disable-shared --prefix="$PREFIX" \
  --without-pthreads \
  --disable-ssp --disable-asm --disable-pie &&
  emmake make clean
[ $? = 0 ] || exit 1

if [ "$DIST" = yes ]; then
  emccLibsodium() {
    outFile="${1}"
    shift
    emcc "$CFLAGS" --llvm-lto 1 $CPPFLAGS $LDFLAGS $JS_EXPORTS_FLAGS "${@}" \
      "${PREFIX}/lib/libsodium.a" -o "${outFile}" || exit 1
  }
  emmake make $MAKE_FLAGS install || exit 1
  emccLibsodium "${PREFIX}/lib/libsodium.asm.tmp.js" -Oz -s WASM=0 $LDFLAGS_JS
  emccLibsodium "${PREFIX}/lib/libsodium.wasm.tmp.js" -O3 -s WASM=1 -s EVAL_CTORS=1 -s INITIAL_MEMORY=${WASM_INITIAL_MEMORY}

  # Build the output file by concatenating parts to preserve null bytes
  # (command substitution in heredoc strips null bytes from WASM binary)
  #
  # Structure:
  # 1. pre.tmp.js - module header, opens useBackupModule function
  # 2. asm.tmp.js - asm.js backup code (INSIDE useBackupModule)
  # 3. mid.tmp.js - closes useBackupModule, starts outer scope for WASM
  # 4. wasm.tmp.js - real WASM code (OUTSIDE useBackupModule, at Module.ready level)
  # 5. end.tmp.js - closes Module.ready promise with fallback

  cat >"${PREFIX}/lib/libsodium.pre.tmp.js" <<'PREJS'
    var Module;
    if (typeof Module === 'undefined') {
      Module = {};
    }
    var root = Module;
    if (typeof root['sodium'] !== 'object') {
      if (typeof global === 'object') {
        root = global;
      } else if (typeof window === 'object') {
        root = window;
      }
    }
    if (typeof Module.getRandomValue === 'undefined') {
      try {
        var window_ = 'object' === typeof window ? window : self;
        var crypto_ = typeof window_.crypto !== 'undefined' ? window_.crypto : window_.msCrypto;
        var randomValuesStandard = function() {
          var buf = new Uint32Array(1);
          crypto_.getRandomValues(buf);
          return buf[0] >>> 0;
        };
        randomValuesStandard();
        Module.getRandomValue = randomValuesStandard;
      } catch (e) {
        try {
          var crypto = require('crypto');
          var randomValueNodeJS = function() {
            var buf = crypto['randomBytes'](4);
            return (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) >>> 0;
          };
          randomValueNodeJS();
          Module.getRandomValue = randomValueNodeJS;
        } catch (e) {
          throw 'No secure random number generator found';
        }
      }
    }
    var _Module = Module;
    Module.ready = new Promise(function(resolve, reject) {
      var Module = _Module;
      Module.onAbort = reject;
      Module.print = function(what) {
        typeof(console) !== 'undefined' && console.log(what);
      }
      Module.printErr = function(what) {
        typeof(console) !== 'undefined' && console.warn(what);
      }
      Module.onRuntimeInitialized = function() {
        try {
          /* Test arbitrary wasm function */
          Module._crypto_secretbox_keybytes();
          resolve();
        } catch (err) {
          reject(err);
        }
      };
      Module.useBackupModule = function() {
        return new Promise(function(resolve, reject) {
          var Module = {};
          Module.onAbort = reject;
          Module.getRandomValue = _Module.getRandomValue;

          Module.onRuntimeInitialized = function() {
            Object.keys(_Module).forEach(function(k) {
              if (k !== 'getRandomValue') {
                delete _Module[k];
              }
            });
            Object.keys(Module).forEach(function(k) {
              _Module[k] = Module[k];
            });
            resolve();
          };

PREJS

  cat >"${PREFIX}/lib/libsodium.mid.tmp.js" <<'MIDJS'
        });
      };
MIDJS

  cat >"${PREFIX}/lib/libsodium.end.tmp.js" <<'ENDJS'
    }).catch(function() {
      return _Module.useBackupModule();
    });
ENDJS

  # Concatenate all parts, preserving binary content
  # Note: asm.js goes inside useBackupModule, wasm.js goes outside at Module.ready level
  cat "${PREFIX}/lib/libsodium.pre.tmp.js" > "${PREFIX}/lib/libsodium.js"
  sed "s|use asm||g" "${PREFIX}/lib/libsodium.asm.tmp.js" >> "${PREFIX}/lib/libsodium.js"
  cat "${PREFIX}/lib/libsodium.mid.tmp.js" >> "${PREFIX}/lib/libsodium.js"
  cat "${PREFIX}/lib/libsodium.wasm.tmp.js" >> "${PREFIX}/lib/libsodium.js"
  cat "${PREFIX}/lib/libsodium.end.tmp.js" >> "${PREFIX}/lib/libsodium.js"

  rm "${PREFIX}/lib/libsodium.pre.tmp.js" "${PREFIX}/lib/libsodium.mid.tmp.js" "${PREFIX}/lib/libsodium.end.tmp.js"

  #KEEP rm "${PREFIX}/lib/libsodium.asm.tmp.js" "${PREFIX}/lib/libsodium.wasm.tmp.js"
  touch -r "${PREFIX}/lib/libsodium.js" "$DONE_FILE"
  ls -l "${PREFIX}/lib/libsodium.js"
  exit 0
fi

if test "$JS_RUNTIME" = ""; then
  for candidate in bun nodejs node /usr/local/bin/bun /usr/local/bin/nodejs /usr/local/bin/node; do
    if command -v $candidate >/dev/null; then
      JS_RUNTIME=$candidate
      break
    fi
  done
fi

find test/default -type f -name '*.js' -exec rm -f {} \;

if [ "x$BROWSER_TESTS" != "x" ]; then
  echo 'Compiling the test suite for web browsers...' &&
    emmake make $MAKE_FLAGS CPPFLAGS="$CPPFLAGS -DBROWSER_TESTS=1" check >/dev/null 2>&1
else
  if test "$JS_RUNTIME" = ""; then
    echo 'Javascript runtime not found - test suite skipped' >&2
    exit 1
  fi
  echo "Using [${JS_RUNTIME}] as a Javascript runtime"
  echo 'Compiling the test suite...' &&
    emmake make $MAKE_FLAGS check >/dev/null 2>&1
fi

if [ "x$BROWSER_TESTS" != "x" ]; then
  echo 'Creating the test suite for web browsers'
  (
    cd test/default &&
      mkdir -p browser &&
      rm -f browser/tests.txt &&
      for file in *.js; do
        cp "$file" "browser/${file}"
        tname=$(echo "$file" | sed 's/.js$//')
        cp -f "${tname}.exp" "browser/${tname}.exp"
        sed "s/{{tname}}/${tname}/" index.html.tpl >"browser/${tname}.html"
        echo "${tname}.html" >>"browser/tests.txt"
      done
    touch "$DONE_FILE"
  )
else
  echo 'Running the test suite'
  (
    cd test/default &&
      for file in *.js; do
        echo "#! /usr/bin/env ${JS_RUNTIME}" >"${file}.tmp"
        cat "$file" >> "${file}.tmp"
        chmod +x "${file}.tmp"
        mv -f "${file}.tmp" "$file"
      done
  )
  make $MAKE_FLAGS check || exit 1
  touch "$DONE_FILE"
fi

echo 'Done.'
