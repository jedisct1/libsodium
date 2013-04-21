#ifndef crypto_core_salsa20_H
#define crypto_core_salsa20_H

#define crypto_core_salsa20_OUTPUTBYTES 64
#define crypto_core_salsa20_INPUTBYTES 16
#define crypto_core_salsa20_KEYBYTES 32
#define crypto_core_salsa20_CONSTBYTES 16

#ifdef __cplusplus
extern "C" {
#endif

int crypto_core_salsa20_ref(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa20 crypto_core_salsa20_ref

#endif
