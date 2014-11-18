var {var_name}_byte;
if ({var_name} instanceof Uint8Array){
	if ({var_name}.length != {var_size}) throw new TypeError('invalid {var_name} size');
	{var_name}_byte = injectBytes({var_name});
} else if (is_hex({var_name})){
	if ({var_name}.length / 2 != {var_size}) throw new TypeError('invalid {var_name} size');
	{var_name}_byte = injectBytes(from_hex({var_name}));
} else {
	if (encode_utf8({var_name}).length != {var_size}) throw new TypeError('invalid {var_name} size');
	{var_name}_byte = injectBytes(encode_utf8({var_name}));
}
