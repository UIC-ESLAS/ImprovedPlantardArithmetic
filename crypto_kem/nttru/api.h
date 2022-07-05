#ifndef API_H
#define API_H

#include "params.h"

#define CRYPTO_SECRETKEYBYTES SECRETKEYBYTES
#define CRYPTO_PUBLICKEYBYTES PUBLICKEYBYTES
#define CRYPTO_CIPHERTEXTBYTES CIPHERTEXTBYTES
#define CRYPTO_BYTES SHAREDKEYBYTES

#define CRYPTO_ALGNAME "NTTRU"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#endif
