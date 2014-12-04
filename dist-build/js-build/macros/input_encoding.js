if ({var_name} && typeof {var_name} != 'string') throw new TypeError('When defined, {var_name} must be a string');
if ({var_name} && !is_encoding({var_name})) throw new Error('{var_name} is not a valid encoding');
