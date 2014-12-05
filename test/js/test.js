var libsodium_test = (function(){

	var t = {};
	var to_hex = libsodium.to_hex;
	var from_hex = libsodium.from_hex;

	t.scrypt = function(){
		if (typeof libsodium.crypto_pwhash_scryptsalsa208sha256 != 'function')
			throw new Error('scrypt cannot be found in the js-built library');

		var vectors = [
			{
				pass: 'password',
				salt: 'NaCl',
				keyLength: 64,
				r: 8,
				p: 16,
				opsLimit: 1024,
				result: 'fdbabe1c9d3472007856e7190d01e9fe7c6ad7cbc8237830e77376634b3731622eaf30d92e22a3886ff109279d9830dac727afb94a83ee6d8360cbdfa2cc0640'
			},
			{
				pass: 'pleaseletmein',
				salt: 'SodiumChloride',
				keyLength: 64,
				r: 8,
				p: 1,
				opsLimit: 16384,
				result: '7023bdcb3afd7348461c06cd81fd38ebfda8fbba904f8e3ea9b543f6545da1f2d5432955613f0fcf62d49705242a9af9e61e85dc0d651e40dfcf017b45575887'
			}
		];

		vectors.forEach(testVector);

		function testVector(v){
			var derivedKey = libsodium.crypto_pwhash_scryptsalsa208sha256_ll(v.pass, v.salt, v.opsLimit, v.r, v.p, v.keyLength);
			if (v.result != to_hex(derivedKey)){
				throw new Error('Error in pwhash_scryptsalsa208sha256_ll with vector ' + JSON.stringify(v));
			}
		}

	};

	t.scalarmult = function(){

		function scalarmult_base_test(){
			var secretKey = '5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb';
			var publicKey = libsodium.crypto_scalarmult_base(from_hex(secretKey));
			var expectedPubKey = 'de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f';
			if (to_hex(publicKey) != expectedPubKey) throw new Error('Error in scalarmult_base');
		}
		scalarmult_base_test();

		var scalarmult_vectors = [
			{
				secretKey: '77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a',
				publicKey: 'de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f',
				sharedSecret: '4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742'
			},
			{
				secretKey: '5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb',
				publicKey: '8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a',
				sharedSecret: '4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742'
			}
		];

		function scalarmult_test(v){
			var sharedSecret = libsodium.crypto_scalarmult(from_hex(v.secretKey), from_hex(v.publicKey));
			if (v.sharedSecret != to_hex(sharedSecret)) throw new Error('Error in scalarmult with vector ' + JSON.stringify(v));
		}
		scalarmult_vectors.forEach(scalarmult_test);

	};

	t.sign = function(callback){
		if (typeof callback != 'function') throw new TypeError('callback must be defined and must be a function');
		var loadVectorsReq = new XMLHttpRequest();
		loadVectorsReq.open('get', 'ed-vectors.json', true);
		loadVectorsReq.onload = function(e){
			var vData = loadVectorsReq.responseText;
			try {
				vData = JSON.parse(vData);
			} catch (e){
				callback(new Error('cannot parse ed25519 vectors'));
				return;
			}
			if (!Array.isArray(vData)){
				callback(new Error('invalid ed25519 vectors list'));
				return;
			}
			var currentVector;
			var nTests = 64;
			console.log('Testing ' + nTests + ' Ed25519 vectors');
			var testCount = 0;
			try {
				while (testCount < nTests){
					currentVector = vData[Math.floor(Math.random() * vData.length)];
					testVector(currentVector);
					testCount++;
				}
			} catch (e){
				console.log('Error on test no. ' + testCount);
				callback(e + ' ' + JSON.stringify(currentVector));
				return;
			}
			callback(undefined, {tested: nTests, count: vData.length});

			function testVector(v){
				var publicKey = v.sk.substr(64); //libsodium.crypto_sign_ed25519_sk_to_pk(v.sk);
				if (publicKey != v.pk) throw new Error('error with sk to pk derivation in Ed25519');
				var signature = libsodium.crypto_sign_detached(from_hex(v.m), from_hex(v.sk));
				if (to_hex(signature) != v.s) throw new Error('unexpected signature in Ed25519');
				var isValid = libsodium.crypto_sign_verify_detached(signature, from_hex(v.m), from_hex(publicKey));
				if (!isValid) throw new Error('invalid signature in Ed25519');
			}
		};
		loadVectorsReq.send();

	};

	return t;
})();
