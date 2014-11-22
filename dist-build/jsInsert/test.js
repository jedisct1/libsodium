//----------------------------------------------------------------------
//-- Functions to test the LibsodiumJS
//----------------------------------------------------------------------


var libsodium = libsodium_factory.instantiate();


// Scrypt (low level function crypto_scrypt_ll)
//----------------------------------------------------------------------
var AAA_PASS  = "password";
var AAA_SALT  = "NaCl";
var AAA_N     = 64; //1024
var AAA_r     = 8;
var AAA_p     = 16;
var AAA_olen  = 64;

                  //crypto_scrypt_ll(password, salt,     N,     r,     p,     olen)
var AAA = libsodium.crypto_scrypt_ll(AAA_PASS, AAA_SALT, AAA_N, AAA_r, AAA_p, AAA_olen);
console.log((AAA));


// Scrypt (High level function crypto_scrypt)
//----------------------------------------------------------------------
var AAB_PASS     = "aaaaa";
var AAB_SALT     = "bbbbb";
var AAB_OPSLIMIT = 481326;
var AAB_MEMLIMIT = 7256678;
var AAB_olen     = 64;

//crypto_pwhash_scryptsalsa208sha256(password, salt, opsLimit, memLimit, olen)
var AAB =    libsodium.crypto_scrypt(AAB_PASS, AAB_SALT, AAB_OPSLIMIT, AAB_MEMLIMIT, AAB_olen)
console.log((AAB));


//CyptoStream (crypto_stream_xor)
//----------------------------------------------------------------------
var AAC_the_message    = libsodium.encode_utf8("Cryptographers Gods exists in this world...");
var AAC_message_lenght = AAC_the_message.length;
var AAC_the_nonce      = libsodium.encode_utf8("With unknown nonces.....");         //crypto_stream_NONCEBYTES (24 bytes)
var AAC_secret_key     = libsodium.encode_utf8("And with infernal passwords....."); //crypto_stream_KEYBYTES   (32 bytes)

var AAC = libsodium.crypto_stream_xor(AAC_the_message, AAC_the_nonce, AAC_secret_key);
var AAC = libsodium.crypto_stream_xor(AAC,             AAC_the_nonce, AAC_secret_key);

console.log("Symmetric encrypted text, decrypted: " +libsodium.decode_utf8(AAC));


//Crypto Box (crypto_box)
//----------------------------------------------------------------------
var AAD_kp           = libsodium.crypto_box_keypair_from_seed(libsodium.encode_utf8("Hola!"));         
var public_cert_box  = AAD_kp.boxPk;         
var private_cert_box = AAD_kp.boxSk;

console.log("Public Crypto Box:");
console.log(public_cert_box);
console.log("Private Crypto Box:");
console.log(private_cert_box);

var AAD_plain_text = libsodium.encode_utf8("I like binary curves...");
var AAD_iv         = libsodium.encode_utf8("It must be 24 bytes long");             //(24 bytes)

try{
    var AAD_sal = libsodium.crypto_box(AAD_plain_text, AAD_iv, public_cert_box, private_cert_box);
}catch(err) {
    console.log("AAD_sal: "+err);
}

try{
    var AAD_m   = libsodium.crypto_box_open(AAD_sal,   AAD_iv, public_cert_box, private_cert_box);
}catch(err) {
    console.log("AAD_m: "+err);
}

console.log("Asymmetric encrypted text, decrypted: " +libsodium.decode_utf8(AAD_m));


//Crypto Sign Box
//----------------------------------------------------------------------
var AAE_kp               = libsodium.crypto_sign_keypair_from_seed(libsodium.encode_utf8("This is a very random seed! ;)"));         
var public_cert_sign     = AAE_kp.signPk;         
var private_cert_sign    = AAE_kp.signSk;  

console.log("Public Crypto Sign Box:");
console.log(public_cert_sign);
console.log("Private Crypto Sign Box:");
console.log(private_cert_sign);

var AAE_plain_text = libsodium.encode_utf8("This is a very secret message that want to be signed! :)");
var AAE_iv         = libsodium.encode_utf8("It must be 24 bytes long");       //(24 bytes)

try{
    var AAE_sal = libsodium.crypto_sign_detached(AAE_plain_text, private_cert_sign);
}catch(err) {
    console.log("AAE_sal: "+err);
}

try{
    var AAE_m   = libsodium.crypto_sign_verify_detached(AAE_sal, AAE_plain_text, public_cert_sign);
}catch(err) {
    console.log("AAE_m: "+err);
}

console.log("Correct signature?: " + AAE_m);


//----------------------------------------------------------------------

//Crypto Hash

var AAAA1_outlen = 64;
var AAAA1_input  = libsodium.encode_utf8("Haseame esta perra!  XD")
var AAAA1_key    = libsodium.encode_utf8("Pero no te vas a dar cuenta de que te meto! ;) ")

var AAAA1_out    = libsodium.crypto_generichash( AAAA1_outlen, AAAA1_input, AAAA1_key );

console.log(AAAA1_out);


//TO DO:
//int crypto_generichash_init(crypto_generichash_state *state, key, keylen, outlen);
//int crypto_generichash_update(crypto_generichash_state *state, in, inlen);
//int crypto_generichash_final(crypto_generichash_state *state, out, outlen);



