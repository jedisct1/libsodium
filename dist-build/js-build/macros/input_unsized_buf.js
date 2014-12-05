var {var_name}_byte;
var {var_name}_size = {var_name}.length;
if ({var_name} instanceof Uint8Array){
	{var_name}_byte = injectBytes({var_name});
} else if (typeof {var_name} == 'string'){
	{var_name}_byte = injectBytes(encode_utf8({var_name}));
} else throw new TypeError('Illegal input type for {var_name}');
