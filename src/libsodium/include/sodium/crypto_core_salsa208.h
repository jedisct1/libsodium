#ifndef crypto_core_salsa208_H
#define crypto_core_salsa208_H

#define crypto_core_salsa208_OUTPUTBYTES 64
#define crypto_core_salsa208_INPUTBYTES 16
#define crypto_core_salsa208_KEYBYTES 32
#define crypto_core_salsa208_CONSTBYTES 16

#ifdef __cplusplus
extern "C" {
#endif

int crypto_core_salsa208_ref(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa208 crypto_core_salsa208_ref

#endif
