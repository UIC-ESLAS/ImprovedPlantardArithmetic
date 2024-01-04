#ifndef PACKING_H
#define PACKING_H

#include <stdint.h>
#include "params.h"
#include "polyvec.h"
#include "smallpoly.h"

#define pack_pk DILITHIUM_NAMESPACE(pack_pk)
void pack_pk(uint8_t pk[CRYPTO_PUBLICKEYBYTES], const uint8_t rho[SEEDBYTES], const polyveck *t1);

#define pack_pk_rho DILITHIUM_NAMESPACE(pack_pk_rho)
void pack_pk_rho(unsigned char pk[CRYPTO_PUBLICKEYBYTES],
                 const unsigned char rho[SEEDBYTES]);

#define getoffset_pk_rho DILITHIUM_NAMESPACE(getoffset_pk_rho)
const unsigned char *getoffset_pk_rho(const unsigned char pk[CRYPTO_PUBLICKEYBYTES]);

#define pack_pk_t1 DILITHIUM_NAMESPACE(pack_pk_t1)
void pack_pk_t1(unsigned char pk[CRYPTO_PUBLICKEYBYTES],
                const poly *t1,
                const unsigned int idx);

#define pack_sk DILITHIUM_NAMESPACE(pack_sk)
void pack_sk(uint8_t sk[CRYPTO_SECRETKEYBYTES],
             const uint8_t rho[SEEDBYTES],
             const uint8_t tr[SEEDBYTES],
             const uint8_t key[SEEDBYTES],
             const polyveck *t0,
             const polyvecl *s1,
             const polyveck *s2);

#define pack_sk_s1 DILITHIUM_NAMESPACE(pack_sk_s1)
void pack_sk_s1(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *s1_elem,
                const unsigned int idx);

#define pack_sk_s2 DILITHIUM_NAMESPACE(pack_sk_s2)
void pack_sk_s2(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *s2_elem,
                const unsigned int idx);

#define pack_sk_t0 DILITHIUM_NAMESPACE(pack_sk_t0)
void pack_sk_t0(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *t0_elem,
                const unsigned int idx);

#define pack_sk_rho DILITHIUM_NAMESPACE(pack_sk_rho)
void pack_sk_rho(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                 const unsigned char rho[SEEDBYTES]);

#define pack_sk_key DILITHIUM_NAMESPACE(pack_sk_key)
void pack_sk_key(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                 const unsigned char key[SEEDBYTES]);

#define pack_sk_tr DILITHIUM_NAMESPACE(pack_sk_tr)
void pack_sk_tr(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const unsigned char tr[SEEDBYTES]);

#define pack_sig DILITHIUM_NAMESPACE(pack_sig)
void pack_sig(uint8_t sig[CRYPTO_BYTES], const uint8_t c[SEEDBYTES], const polyvecl *z, const polyveck *h);

#define unpack_pk DILITHIUM_NAMESPACE(unpack_pk)
void unpack_pk(uint8_t rho[SEEDBYTES], polyveck *t1, const uint8_t pk[CRYPTO_PUBLICKEYBYTES]);

#define unpack_sk DILITHIUM_NAMESPACE(unpack_sk)
void unpack_sk(uint8_t rho[SEEDBYTES],
               uint8_t tr[SEEDBYTES],
               uint8_t key[SEEDBYTES],
               polyveck *t0,
               smallpoly s1[L],
               smallpoly s2[K],
               const uint8_t sk[CRYPTO_SECRETKEYBYTES]);

#define unpack_sig DILITHIUM_NAMESPACE(unpack_sig)
int unpack_sig(uint8_t c[SEEDBYTES], polyvecl *z, polyveck *h, const uint8_t sig[CRYPTO_BYTES]);

#define unpack_sig_z DILITHIUM_NAMESPACE(unpack_sig_z)
int unpack_sig_z(polyvecl *z, const unsigned char sig[CRYPTO_BYTES]);

#define unpack_sig_h DILITHIUM_NAMESPACE(unpack_sig_h)
int unpack_sig_h(poly *h, unsigned int idx, const unsigned char sig[CRYPTO_BYTES]);

#define unpack_pk_t1 DILITHIUM_NAMESPACE(unpack_pk_t1)
void unpack_pk_t1(poly *t1, unsigned int idx, const unsigned char pk[CRYPTO_PUBLICKEYBYTES]);

#define pack_sig_c DILITHIUM_NAMESPACE(pack_sig_c)
void pack_sig_c(uint8_t sig[CRYPTO_BYTES], const uint8_t c[SEEDBYTES]);

#define pack_sig_z DILITHIUM_NAMESPACE(pack_sig_z)
void pack_sig_z(uint8_t sig[CRYPTO_BYTES], const polyvecl *z);

#define pack_sig_h DILITHIUM_NAMESPACE(pack_sig_h)
void pack_sig_h(unsigned char sig[CRYPTO_BYTES],
                const poly *h_elem,
                const unsigned int idx,
                unsigned int *hints_written);

#define pack_sig_h_zero DILITHIUM_NAMESPACE(pack_sig_h_zero)
void pack_sig_h_zero(unsigned char sig[CRYPTO_BYTES],
                     unsigned int *hints_written);

#endif