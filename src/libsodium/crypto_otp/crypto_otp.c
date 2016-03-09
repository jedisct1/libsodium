#include <stdio.h>
#include <inttypes.h>

#include "crypto_auth_hmacsha256.h"
#include "crypto_auth_hmacsha512.h"
#include "crypto_otp.h"
#include "utils.h"

static unsigned int DIGITS_POWER[] = {
    1000000, 10000000, 100000000
};

static inline void
crypto_hotp_truncate(const char* hash, unsigned int offset, unsigned int digits, char* out) {

    // And extracts the code from the hash
    unsigned int n = ((hash[offset] & 0x7f) << 24) |
                     ((hash[offset + 1] & 0xff) << 16) |
                     ((hash[offset + 2] & 0xff) << 8) |
                     (hash[offset + 3] & 0xff);
    n = n % DIGITS_POWER[digits - 6];

    // Converts the final number to a string
    // and pads the otput to the required number of digits
    sprintf(out, "%0*d", digits, n);
}

// --------------------------------------------------------------

int
crypto_hotp(const char* key, size_t keySize, uint64_t counter, unsigned int digits, char* out) {
    return crypto_hotp_sha512(key, keySize, counter, digits, out);
}

int
crypto_hotp_verify(const char* key, size_t keySize, uint64_t counter, unsigned int digits, unsigned int window, const char* m) {
    return crypto_hotp_sha512_verify(key, keySize, counter, digits, window, m);
}

int
crypto_hotp_sha256(const char* key, size_t keySize, uint64_t counter, unsigned int digits, char* out) {
    unsigned char hash[crypto_auth_hmacsha256_BYTES];
    crypto_auth_hmacsha256_state state;
    unsigned char counter_bytes[8];

    // Checks the number of digits
    if (digits < 6 || digits > 8)
        return -1;

    // Converts the counter to a byte array in big endian
    counter_bytes[7] = counter >> 0;
    counter_bytes[6] = counter >> 8;
    counter_bytes[5] = counter >> 16;
    counter_bytes[4] = counter >> 24;
    counter_bytes[3] = counter >> 32;
    counter_bytes[2] = counter >> 40;
    counter_bytes[1] = counter >> 48;
    counter_bytes[0] = counter >> 56;

    // Now we compute the HMAC and truncate it
    crypto_auth_hmacsha256_init(&state, key, keySize);
    crypto_auth_hmacsha256_update(&state, counter_bytes, 8);
    crypto_auth_hmacsha256_final(&state, hash);
    unsigned int offset = hash[crypto_auth_hmacsha256_BYTES - 1] & 0xf;
    crypto_hotp_truncate(hash, offset, digits, out);

    return 0;
}

int
crypto_hotp_sha256_verify(const char* key, size_t keySize, uint64_t counter, unsigned int digits, unsigned int window, const char* m) {
   
    if (digits < 6 || digits > 8)
        return -1;

    int i;
    char buf[digits + 1];
    for (i = counter - window; i <= counter + window; i++) {
        crypto_hotp_sha256(key, keySize, i, digits, buf);
        if (sodium_memcmp(m, buf, digits) == 0)
            return 0;
    }

    return -1;

}

int
crypto_hotp_sha512(const char* key, size_t keySize, uint64_t counter, unsigned int digits, char* out) {
    unsigned char hash[crypto_auth_hmacsha512_BYTES];
    crypto_auth_hmacsha512_state state;
    unsigned char counter_bytes[8];

    // Checks the number of digits
    if (digits < 6 || digits > 8)
        return -1;

    // Converts the counter to a byte array in big endian
    counter_bytes[7] = counter >> 0;
    counter_bytes[6] = counter >> 8;
    counter_bytes[5] = counter >> 16;
    counter_bytes[4] = counter >> 24;
    counter_bytes[3] = counter >> 32;
    counter_bytes[2] = counter >> 40;
    counter_bytes[1] = counter >> 48;
    counter_bytes[0] = counter >> 56;

    // Now we compute the HMAC and truncate it
    crypto_auth_hmacsha512_init(&state, key, keySize);
    crypto_auth_hmacsha512_update(&state, counter_bytes, 8);
    crypto_auth_hmacsha512_final(&state, hash);
    unsigned int offset = hash[crypto_auth_hmacsha512_BYTES - 1] & 0xf;
    crypto_hotp_truncate(hash, offset, digits, out);

    return 0;
}

int
crypto_hotp_sha512_verify(const char* key, size_t keySize, uint64_t counter, unsigned int digits, unsigned int window, const char* m) {

    if (digits < 6 || digits > 8)
        return -1;

    int i;
    char buf[digits + 1];
    for (i = counter - window; i <= counter + window; i++) {
        crypto_hotp_sha512(key, keySize, i, digits, buf);
        if (sodium_memcmp(m, buf, digits) == 0)
            return 0;
    }

    return -1;

}

// --------------------------------------------

int
crypto_totp(const char* key, size_t keySize, uint64_t now, unsigned int digits, char* out) {
    return crypto_totp_sha512(key, keySize, now, 0, 30, digits, out);
}

int
crypto_totp_verify(const char* key, size_t keySize, uint64_t now, unsigned int digits, unsigned int window, const char* m) {
    return crypto_totp_sha512_verify(key, keySize, now, 0, 30, digits, window, m);
}

int
crypto_totp_sha256(const char* key, size_t keySize, uint64_t now, uint64_t initialTime, uint64_t step, unsigned int digits, char* out) {
    return crypto_hotp_sha256(key, keySize, (now - initialTime) / step, digits, out);
}

int
crypto_totp_sha256_verify(const char* key, size_t keySize, uint64_t now, uint64_t initialTime, uint64_t step, unsigned int digits, unsigned int window, const char* m) {
    return crypto_hotp_sha256_verify(key, keySize, (now - initialTime) / step, digits, window, m);
}

int
crypto_totp_sha512(const char* key, size_t keySize, uint64_t now, uint64_t initialTime, uint64_t step, unsigned int digits, char* out) {
    return crypto_hotp_sha512(key, keySize, (now - initialTime) / step, digits, out);
}

int
crypto_totp_sha512_verify(const char* key, size_t keySize, uint64_t now, uint64_t initialTime, uint64_t step, unsigned int digits, unsigned int window, const char* m) {
    return crypto_hotp_sha512_verify(key, keySize, (now - initialTime) / step, digits, window, m);
}

