#include "randombytes.h"
#include "params.h"
#include "poly.h"
#include "hal.h"
#include "sendfn.h"

#define printcycles(S, U) send_unsignedll((S), (U))
int ntru_keygen(poly *hhat, poly *fhat, const unsigned char coins[N]) {
  int r=0;
  poly f, g;
  // printcycles("ntru_keygen", r);
  poly_short(fhat, coins);
  poly_short(&g, coins + N/2);
  poly_triple(&g, &g);
  poly_triple(fhat, fhat);
  fhat->coeffs[0] += 1;
  // Input range:<4bits
  poly_ntt(fhat);
  poly_ntt(&g);
  // poly_reduce(&g);
  poly_freeze(fhat);
  r = poly_baseinv(&f, fhat);//\beta^2 * f^-1
  // printcycles("poly_baseinv:",r);
  poly_basemul(hhat, &f, &g);//beta
  poly_freeze(hhat);
  return r;
}

void ntru_encrypt(poly *chat,
                  poly *m,
                  const poly *hhat,
                  const unsigned char coins[N / 2])
{
  poly r, mhat;
  poly_short(&r, coins);
  poly_ntt(&r);
  poly_ntt(m);
  poly_basemul(chat, &r, hhat);//normal
  poly_reduce(chat);
  poly_add(chat, chat, m);
  poly_freeze(chat);
}

void ntru_decrypt(poly *m,
                  const poly *chat,
                  const poly *fhat)
{
  poly_basemul(m, chat, fhat);//beta^-1
  // poly_reduce(m);//-q,q; should be eliminate: adding here leads to incorrect; the input of invntt should be (-0.5q,0.5q)
  
  poly_invntt(m);
  poly_crepmod3(m, m);
}
