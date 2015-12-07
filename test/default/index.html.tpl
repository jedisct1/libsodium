<!doctype html>
<html>
<head>
<style>
<meta name="google" content="notranslate" />
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
        if (xhr.readyState != 4 ||
            (xhr.status != 200 && xhr.status != 302 && xhr.status != 0)) {
            return;
        }
        expected = xhr.responseText.split('\n');
        if (expected.length > 0 && expected[expected.length - 1] === '') {
            expected.pop();
        }
        expected.push('--- SUCCESS ---');
        window.addEventListener('test-output', outputReceived, false);
        var s = document.getElementsByTagName('script')[0];
        var st = document.createElement('script');
        st.src = tname + '.js';
        s.parentNode.insertBefore(st, s);
    }
    xhr.send(null);
}
runTest('{{tname}}');
</script>
</body>
</html>
