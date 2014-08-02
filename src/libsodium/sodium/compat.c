
#include "crypto_pwhash_scryptsalsa208sha256.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef crypto_pwhash_scryptxsalsa208sha256_saltbytes
SODIUM_EXPORT size_t
crypto_pwhash_scryptxsalsa208sha256_saltbytes(void)
{
    return crypto_pwhash_scryptsalsa208sha256_saltbytes();
}

#undef crypto_pwhash_scryptxsalsa208sha256_strbytes
SODIUM_EXPORT size_t
crypto_pwhash_scryptxsalsa208sha256_strbytes(void)
{
    return crypto_pwhash_scryptsalsa208sha256_strbytes();
}

#undef crypto_pwhash_scryptxsalsa208sha256
SODIUM_EXPORT int
crypto_pwhash_scryptxsalsa208sha256(unsigned char * const out,
                                    unsigned long long outlen,
                                    const char * const passwd,
                                    unsigned long long passwdlen,
                                    const unsigned char * const salt,
                                    unsigned long long opslimit,
                                    size_t memlimit)
{
    return crypto_pwhash_scryptsalsa208sha256(out, outlen, passwd, passwdlen,
                                              salt, opslimit, memlimit);
}

#undef crypto_pwhash_scryptxsalsa208sha256_str
SODIUM_EXPORT int
crypto_pwhash_scryptxsalsa208sha256_str(char out[crypto_pwhash_scryptsalsa208sha256_STRBYTES],
                                        const char * const passwd,
                                        unsigned long long passwdlen,
                                        unsigned long long opslimit,
                                        size_t memlimit)
{
    return crypto_pwhash_scryptsalsa208sha256_str(out, passwd, passwdlen,
                                                  opslimit, memlimit);
}

#undef crypto_pwhash_scryptxsalsa208sha256_str_verify
SODIUM_EXPORT int
crypto_pwhash_scryptxsalsa208sha256_str_verify(const char str[crypto_pwhash_scryptsalsa208sha256_STRBYTES],
                                               const char * const passwd,
                                               unsigned long long passwdlen)
{
    return crypto_pwhash_scryptsalsa208sha256_str_verify(str,
                                                         passwd, passwdlen);
}

#ifdef __cplusplus
}
#endif
