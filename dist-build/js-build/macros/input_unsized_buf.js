var {var_name}_byte;
var {var_name}_size = {var_name}.length;
if ({var_name} instanceof Uint8Array){
	{var_name}_byte = injectBytes({var_name});
} else if (is_hex({var_name})){
	{var_name}_byte = injectBytes(from_hex({var_name}));
	{var_name}_size /= 2;
} else {
	{var_name}_byte = injectBytes(encode_utf8({var_name}));
}
