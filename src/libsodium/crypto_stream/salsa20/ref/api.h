
#include "crypto_stream_salsa20.h"

#define crypto_stream_salsa20_implementation_name \
    crypto_stream_salsa20_ref_implementation_name

#define crypto_stream crypto_stream_salsa20_ref
#define crypto_stream_xor crypto_stream_salsa20_ref_xor
