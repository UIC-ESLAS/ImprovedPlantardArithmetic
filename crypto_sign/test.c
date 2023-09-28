#include "api.h"
#include "randombytes.h"
#include "hal.h"
#include "sendfn.h"
#include <string.h>

#define NTESTS 1
#define MLEN 32

const uint8_t canary[8] = {
  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
};

/* allocate a bit more for all keys and messages and
 * make sure it is not touched by the implementations.
 */
static void write_canary(uint8_t *d) {
  for (size_t i = 0; i < 8; i++) {
    d[i] = canary[i];
  }
}

static int check_canary(const uint8_t *d) {
  for (size_t i = 0; i < 8; i++) {
    if (d[i] != canary[i]) {
      return -1;
    }
  }
  return 0;
}


static int test_sign(void)
{
    unsigned char pk[CRYPTO_PUBLICKEYBYTES+16];
    unsigned char sk[CRYPTO_SECRETKEYBYTES+16];
    unsigned char sm[MLEN + CRYPTO_BYTES+16];
    unsigned char m[MLEN+16];

    size_t mlen;
    size_t smlen;

    int i;
    write_canary(pk); write_canary(pk+sizeof(pk)-8);
    write_canary(sk); write_canary(sk+sizeof(sk)-8);
    write_canary(sm); write_canary(sm+sizeof(sm)-8);
    write_canary(m); write_canary(m+sizeof(m)-8);

    for (i = 0; i < NTESTS; i++) {
        crypto_sign_keypair(pk+8, sk+8);
        hal_send_str("crypto_sign_keypair DONE.\n");

        randombytes(m+8, MLEN);
        crypto_sign(sm+8, &smlen, m+8, MLEN, sk+8);
        hal_send_str("crypto_sign DONE.\n");

        // By relying on m == sm we prevent having to allocate CRYPTO_BYTES twice
        if (crypto_sign_open(sm+8, &mlen, sm+8, smlen, pk+8))
        {
            hal_send_str("ERROR Signature did not verify correctly!\n");
        }
        else if(check_canary(pk) || check_canary(pk+sizeof(pk)-8) ||
            check_canary(sk) || check_canary(sk+sizeof(sk)-8) ||
            check_canary(sm) || check_canary(sm+sizeof(sm)-8) ||
            check_canary(m) || check_canary(m+sizeof(m)-8))
        {
            hal_send_str("ERROR canary overwritten\n");
        }
        else
        {
            hal_send_str("OK Signature did verify correctly!\n");
        }
        hal_send_str("crypto_sign_open DONE.\n");
    }

    return 0;
}

static int test_wrong_pk(void)
{
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char pk2[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char sm[MLEN + CRYPTO_BYTES];
    unsigned char m[MLEN];

    size_t mlen;
    size_t smlen;

    int i;

    for (i = 0; i < NTESTS; i++) {
        crypto_sign_keypair(pk2, sk);
        hal_send_str("crypto_sign_keypair DONE.\n");

        crypto_sign_keypair(pk, sk);
        hal_send_str("crypto_sign_keypair DONE.\n");


        randombytes(m, MLEN);
        crypto_sign(sm, &smlen, m, MLEN, sk);
        hal_send_str("crypto_sign DONE.\n");

        // By relying on m == sm we prevent having to allocate CRYPTO_BYTES twice
        if (crypto_sign_open(sm, &mlen, sm, smlen, pk2))
        {
            hal_send_str("OK Signature did not verify correctly under wrong public key!\n");
        }
        else
        {
            hal_send_str("ERROR Signature did verify correctly under wrong public key!\n");
        }
        hal_send_str("crypto_sign_open DONE.\n");
    }

    return 0;
}

#include "ntt.h"
#include "smallntt.h"
#include "smallpoly.h"
#include "poly.h"
#include "polyvec.h"
static void printbytes(const unsigned char *x, unsigned long long xlen)
{
    char outs[2 * xlen + 1];
    unsigned long long i;
    for (i = 0; i < xlen; i++)
        sprintf(outs + 2 * i, "%02x", x[i]);
    outs[2 * xlen] = 0;
    hal_send_str(outs);
}

static void printpoly32(const poly *x)
{
    char outs[4 *N + 1];
    unsigned long long i;
    for (i = 0; i < N; i++)
        sprintf(outs + 4 * i, "%04x", x->coeffs[i]);
    outs[4 * N] = 0;
    hal_send_str(outs);
}
static void printpoly16(const poly *x)
{
  int16_t *xp=(int16_t*)x->coeffs;
    char outs[4 * N + 1];
    unsigned long long i;
    for (i = 0; i < 2*N; i++)
        sprintf(outs + 2 * i, "%02x", xp[i]);
    outs[4 * N] = 0;
    hal_send_str(outs);
}

int test_double_moduli_ntt(){
  poly a,a1,b,c,cprime;
  int16_t *bp = (int16_t *)b.coeffs;

  // Use 32-bit NTT
  poly_challenge(&c,"random sign");
  for (int i = 0; i < N; i++)
  {
        b.coeffs[i] = i + (1 << 12);
  }
  // printpoly32(&c);
  // printpoly32(&b);
  poly_ntt(&c);
  poly_ntt(&b);
  poly_pointwise_montgomery(&a, &c, &b);
  poly_invntt_tomont(&a);
  poly_reduce(&a);
  hal_send_str("Use 32-bit NTT output:");
  printpoly32(&a);

  // use multi-moduli NTT
  poly_challenge_new(&c, "random sign");
  for (int i = 0; i < N; i++)
  {
        bp[i] = i+(1<<12);
        bp[i + N] = bp[i];
  }
  poly_double_ntt_precomp(&cprime,&c);
  poly_double_ntt(&b);
  poly_double_basemul_invntt(&a1,&c,&cprime,&b);
  poly_reduce(&a1);
  hal_send_str("Use multi-moduli NTT output:");
  printpoly32(&a1);

  for(int i=0;i<N;i++){
    if(a.coeffs[i]!=a1.coeffs[i]){
      hal_send_str("error");
    }
  }
}


int main(void)
{
    hal_setup(CLOCK_FAST);

    // marker for automated testing
    for(int i = 0; i < 2; i++){
        hal_send_str("==========================");
    }
    test_sign();
    test_wrong_pk();
    // test_double_moduli_ntt();

    hal_send_str("#");

    while(1);
    return 0;
}
