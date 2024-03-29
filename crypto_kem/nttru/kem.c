#include <stddef.h>
#include <sha2.h>
#include "randombytes.h"
// #include "crypto_stream.h"
#include "params.h"
#include "ntru.h"
#include "poly.h"
#include "api.h"
#include "aes256ctr.h"

static const unsigned char n = 0;

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk) {
  unsigned int i;
  unsigned char coins[N];
  poly hhat, fhat;

  do {
    randombytes(coins, 32);
    aes256_prf(coins, N, coins, n);
  } while(ntru_keygen(&hhat, &fhat, coins));

  poly_pack_uniform(pk, &hhat);
  poly_pack_uniform(sk, &fhat);
  for(i = 0; i < PUBLICKEYBYTES; ++i)
    sk[i + POLY_PACKED_UNIFORM_BYTES] = pk[i];

  return 0;
}

int crypto_kem_enc(unsigned char *c, unsigned char *k, const unsigned char *pk) {
  unsigned int i;
  unsigned char buf[32 + COINBYTES];
  poly hhat, chat, m;

  randombytes(buf, 32);
  aes256_prf(buf, N/2, buf, n);

  poly_short(&m, buf);
  poly_pack_short(buf, &m);
  sha512(buf, buf, MSGBYTES);
  aes256_prf(buf + 32, COINBYTES, buf + 32, n);

  poly_unpack_uniform(&hhat, pk);
  ntru_encrypt(&chat, &m, &hhat, buf + 32);
  poly_pack_uniform(c, &chat);

  for (i = 0; i < SHAREDKEYBYTES; ++i)
    k[i] = buf[i];

  return 0;
}

int crypto_kem_dec(unsigned char *k,
                   const unsigned char *c,
                   const unsigned char *sk)
{
  unsigned int i;
  unsigned char buf[32 + COINBYTES];
  int16_t t;
  int32_t fail;
  poly m, hhat, chat, fhat;

  poly_unpack_uniform(&chat, c);
  poly_unpack_uniform(&fhat, sk);
  ntru_decrypt(&m, &chat, &fhat);

  poly_pack_short(buf, &m);
  sha512(buf, buf, MSGBYTES);
  aes256_prf(buf + 32, COINBYTES, buf + 32, n);

  poly_unpack_uniform(&hhat, sk + POLY_PACKED_UNIFORM_BYTES);
  ntru_encrypt(&fhat, &m, &hhat, buf + 32);

  t = 0;
  for(i = 0; i < N; ++i)
    t |= chat.coeffs[i] ^ fhat.coeffs[i];

  fail = (uint16_t)t;
  fail = (-fail) >> 31;
  for(i = 0; i < SHAREDKEYBYTES; ++i)
    k[i] = buf[i] & ~(-fail);

  return fail;
}
