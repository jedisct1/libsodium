var libsodium_factory = { 
    instantiate: function ( requested_total_memory ) {
        return (function ( window, document ) {
            var Module = {TOTAL_MEMORY: (requested_total_memory || 33554432)};
            var libsodium_raw = Module;
