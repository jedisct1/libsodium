<!doctype html>

<html>
  <head>
    <meta charset="utf-8" />
    <meta name="google" content="notranslate" />
    <style>
      body {
        background: white;
        color: black;
      }
      .test p {
        margin: 1px;
      }
      .test {
        font-family: monospace;
        white-space: pre;
      }
      .err {
        background: red;
        color: white;
      }
      .passed {
        background: green;
        color: white;
      }
    </style>
  </head>
  <body>
    <h1></h1>
    <section class="test" id="test-res"></section>
    <script>
      window.testMain = function() {
        function runTest(tname) {
          var xhr, expected, hn, idx = 0, passed = true;

          function outputReceived(e) {
            var found = e.data;
            var p = document.createElement('p');
            if (found !== expected[idx++]) {
              p.className = 'err';
              passed = false;
            }
            p.appendChild(document.createTextNode(found));
            document.getElementById('test-res').appendChild(p);
            if (idx >= expected.length) {
              if (passed) {
                hn.appendChild(document.createTextNode(' - PASSED'));
                hn.className = 'passed';
              } else {
                hn.appendChild(document.createTextNode(' - FAILED'));
                hn.className = 'err';
              }
              if (window.ontestresult) window.ontestresult(passed); // used for the full suite
            }
          }

          hn = document.getElementsByTagName('h1')[0];
          hn.appendChild(document.createTextNode('Test: ' + tname));

          try {
            xhr = new ActiveXObject('Microsoft.XMLHTTP');
          } catch (e) {
            xhr = new XMLHttpRequest();
          }
          xhr.open('GET', tname + '.exp');
          xhr.onreadystatechange = function() {
            if (xhr.readyState != 4 || (xhr.status != 200 && xhr.status != 302 && xhr.status != 0)) return;
            expected = xhr.responseText.split('\n');
            if (expected.length > 0 && expected[expected.length - 1] === '') expected.pop();
            expected.push('--- SUCCESS ---');
            window.addEventListener('test-output', outputReceived, false);
            var wasmXhr = null;
            try {
              wasmXhr = new ActiveXObject('Microsoft.XMLHTTP');
            } catch (e) {
              wasmXhr = new XMLHttpRequest();
            }
            wasmXhr.open('GET', tname + '.wasm');
            wasmXhr.responseType = 'arraybuffer';
            wasmXhr.onreadystatechange = function() {
              if (wasmXhr.readyState != 4 || (wasmXhr.status != 200 && wasmXhr.status != 302 && wasmXhr.status != 0)) return;
              window.Module = window.Module || {};
              Module.wasmBinary = wasmXhr.response;
              var s = document.getElementsByTagName('script')[0];
              var st = document.createElement('script');
              st.src = tname + '.js';
              s.parentNode.insertBefore(st, s);
            }
            wasmXhr.send(null);
          }
          xhr.send(null);
        }
        runTest('{{tname}}');
      };
    </script>
    <script>
      (function() {
        function inIframe() {
          try {
            // foolproof, but modern
            // also might be blocked to prevent XSS
            return !!window.frameElement;
          } catch (e) {
            try {
              // might be blocked by same origin policy
              return window.self !== window.top;
            } catch (e) {
              return true;
            }
          }
        }

        if (!inIframe()) window.testMain(); // run the test automatically if the user loaded the page directly
      })();
    </script>
  </body>
</html>
