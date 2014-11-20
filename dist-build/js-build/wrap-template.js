var libsodium = (function () {
	'use strict';
	var exports = {};

	var libsodium_raw = r;

	//---------------------------------------------------------------------------
	// Horrifying UTF-8 and hex codecs

	function encode_utf8(s) {
	return encode_latin1(unescape(encodeURIComponent(s)));
	}

	function encode_latin1(s) {
	var result = new Uint8Array(s.length);
	for (var i = 0; i < s.length; i++) {
		var c = s.charCodeAt(i);
		if ((c & 0xff) !== c) throw {message: "Cannot encode string in Latin1", str: s};
		result[i] = (c & 0xff);
	}
	return result;
	}

	function decode_utf8(bs) {
	return decodeURIComponent(escape(decode_latin1(bs)));
	}

	function decode_latin1(bs) {
	var encoded = [];
	for (var i = 0; i < bs.length; i++) {
		encoded.push(String.fromCharCode(bs[i]));
	}
	return encoded.join('');
	}

	function to_hex(bs) {
	var encoded = [];
	for (var i = 0; i < bs.length; i++) {
		encoded.push("0123456789abcdef"[(bs[i] >> 4) & 15]);
		encoded.push("0123456789abcdef"[bs[i] & 15]);
	}
	return encoded.join('');
	}

	function from_hex(s) {
		var result = new Uint8Array(s.length / 2);
		for (var i = 0; i < s.length / 2; i++) {
			result[i] = parseInt(s.substr(2*i,2),16);
		}
		return result;
	}

	function is_hex(s){
		return (typeof s == 'string' && /^([a-f]|[0-9])+$/i.test(s) && s.length % 2 == 0);
	}

	//---------------------------------------------------------------------------
	// Allocation

	function MALLOC(nbytes) {
	var result = libsodium_raw._malloc(nbytes);
	if (result === 0) {
		throw {message: "malloc() failed", nbytes: nbytes};
	}
	return result;
	}

	function FREE(pointer) {
	libsodium_raw._free(pointer);
	}

	//---------------------------------------------------------------------------

	function injectBytes(bs, leftPadding) {
	var p = leftPadding || 0;
	var address = MALLOC(bs.length + p);
	libsodium_raw.HEAPU8.set(bs, address + p);
	for (var i = address; i < address + p; i++) {
		libsodium_raw.HEAPU8[i] = 0;
	}
	return address;
	}

	function check_injectBytes(function_name, what, thing, expected_length, leftPadding) {
	check_length(function_name, what, thing, expected_length);
	return injectBytes(thing, leftPadding);
	}

	function extractBytes(address, length) {
	var result = new Uint8Array(length);
	result.set(libsodium_raw.HEAPU8.subarray(address, address + length));
	return result;
	}

	//---------------------------------------------------------------------------

	//Returns the list of functions and constants defined in the wrapped libsodium
	function symbols(){
		return Object.keys(exports).sort();
	}

	function check(function_name, result) {
	if (result !== 0) {
		throw {message: "libsodium_raw." + function_name + " signalled an error"};
	}
	}

	function check_length(function_name, what, thing, expected_length) {
	if (thing.length !== expected_length) {
		throw {message: "libsodium." + function_name + " expected " +
			expected_length + "-byte " + what + " but got length " + thing.length};
	}
	}

	function TargetBuffer(length) {
	this.length = length;
	this.address = MALLOC(length);
	}

	TargetBuffer.prototype.extractBytes = function (offset) {
	var result = extractBytes(this.address + (offset || 0), this.length - (offset || 0));
	FREE(this.address);
	this.address = null;
	return result;
	};

	function free_all(addresses) {
	for (var i = 0; i < addresses.length; i++) {
		FREE(addresses[i]);
	}
	}

	{wraps_here}

	exports.encode_utf8   = encode_utf8;
	exports.encode_latin1 = encode_latin1;
	exports.decode_utf8   = decode_utf8;
	exports.decode_latin1 = decode_latin1;
	exports.to_hex        = to_hex;
	exports.from_hex      = from_hex;
	exports.is_hex        = is_hex;
	exports.symbols       = symbols;

	{exports_here}

	return exports;
})();
