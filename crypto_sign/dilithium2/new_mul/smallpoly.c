#include "smallpoly.h"
#include "symmetric.h"

void small_polyeta_unpack(smallpoly *r, const uint8_t *a) {
  unsigned int i;

#if ETA == 2
  for(i = 0; i < N/8; ++i) {
    r->coeffs[8*i+0] =  (a[3*i+0] >> 0) & 7;
    r->coeffs[8*i+1] =  (a[3*i+0] >> 3) & 7;
    r->coeffs[8*i+2] = ((a[3*i+0] >> 6) | (a[3*i+1] << 2)) & 7;
    r->coeffs[8*i+3] =  (a[3*i+1] >> 1) & 7;
    r->coeffs[8*i+4] =  (a[3*i+1] >> 4) & 7;
    r->coeffs[8*i+5] = ((a[3*i+1] >> 7) | (a[3*i+2] << 1)) & 7;
    r->coeffs[8*i+6] =  (a[3*i+2] >> 2) & 7;
    r->coeffs[8*i+7] =  (a[3*i+2] >> 5) & 7;

    r->coeffs[8*i+0] = ETA - r->coeffs[8*i+0];
    r->coeffs[8*i+1] = ETA - r->coeffs[8*i+1];
    r->coeffs[8*i+2] = ETA - r->coeffs[8*i+2];
    r->coeffs[8*i+3] = ETA - r->coeffs[8*i+3];
    r->coeffs[8*i+4] = ETA - r->coeffs[8*i+4];
    r->coeffs[8*i+5] = ETA - r->coeffs[8*i+5];
    r->coeffs[8*i+6] = ETA - r->coeffs[8*i+6];
    r->coeffs[8*i+7] = ETA - r->coeffs[8*i+7];
  }
#elif ETA == 4
  for(i = 0; i < N/2; ++i) {
    r->coeffs[2*i+0] = a[i] & 0x0F;
    r->coeffs[2*i+1] = a[i] >> 4;
    r->coeffs[2*i+0] = ETA - r->coeffs[2*i+0];
    r->coeffs[2*i+1] = ETA - r->coeffs[2*i+1];
  }
#endif
}

/*************************************************
 * Name:        polyt0_unpack
 *
 * Description: Unpack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
 *
 * Arguments:   - poly *r: pointer to output polynomial
 *              - const uint8_t *a: byte array with bit-packed polynomial
 **************************************************/
void smallpolyt0_unpack(smallpoly *r, const uint8_t *a)
{
  unsigned int i;

  for (i = 0; i < N / 8; ++i)
  {
    r->coeffs[8 * i + 0] = a[13 * i + 0];
    r->coeffs[8 * i + 0] |= (uint32_t)a[13 * i + 1] << 8;
    r->coeffs[8 * i + 0] &= 0x1FFF;

    r->coeffs[8 * i + 1] = a[13 * i + 1] >> 5;
    r->coeffs[8 * i + 1] |= (uint32_t)a[13 * i + 2] << 3;
    r->coeffs[8 * i + 1] |= (uint32_t)a[13 * i + 3] << 11;
    r->coeffs[8 * i + 1] &= 0x1FFF;

    r->coeffs[8 * i + 2] = a[13 * i + 3] >> 2;
    r->coeffs[8 * i + 2] |= (uint32_t)a[13 * i + 4] << 6;
    r->coeffs[8 * i + 2] &= 0x1FFF;

    r->coeffs[8 * i + 3] = a[13 * i + 4] >> 7;
    r->coeffs[8 * i + 3] |= (uint32_t)a[13 * i + 5] << 1;
    r->coeffs[8 * i + 3] |= (uint32_t)a[13 * i + 6] << 9;
    r->coeffs[8 * i + 3] &= 0x1FFF;

    r->coeffs[8 * i + 4] = a[13 * i + 6] >> 4;
    r->coeffs[8 * i + 4] |= (uint32_t)a[13 * i + 7] << 4;
    r->coeffs[8 * i + 4] |= (uint32_t)a[13 * i + 8] << 12;
    r->coeffs[8 * i + 4] &= 0x1FFF;

    r->coeffs[8 * i + 5] = a[13 * i + 8] >> 1;
    r->coeffs[8 * i + 5] |= (uint32_t)a[13 * i + 9] << 7;
    r->coeffs[8 * i + 5] &= 0x1FFF;

    r->coeffs[8 * i + 6] = a[13 * i + 9] >> 6;
    r->coeffs[8 * i + 6] |= (uint32_t)a[13 * i + 10] << 2;
    r->coeffs[8 * i + 6] |= (uint32_t)a[13 * i + 11] << 10;
    r->coeffs[8 * i + 6] &= 0x1FFF;

    r->coeffs[8 * i + 7] = a[13 * i + 11] >> 3;
    r->coeffs[8 * i + 7] |= (uint32_t)a[13 * i + 12] << 5;
    r->coeffs[8 * i + 7] &= 0x1FFF;

    r->coeffs[8 * i + 0] = (1 << (D - 1)) - r->coeffs[8 * i + 0];
    r->coeffs[8 * i + 1] = (1 << (D - 1)) - r->coeffs[8 * i + 1];
    r->coeffs[8 * i + 2] = (1 << (D - 1)) - r->coeffs[8 * i + 2];
    r->coeffs[8 * i + 3] = (1 << (D - 1)) - r->coeffs[8 * i + 3];
    r->coeffs[8 * i + 4] = (1 << (D - 1)) - r->coeffs[8 * i + 4];
    r->coeffs[8 * i + 5] = (1 << (D - 1)) - r->coeffs[8 * i + 5];
    r->coeffs[8 * i + 6] = (1 << (D - 1)) - r->coeffs[8 * i + 6];
    r->coeffs[8 * i + 7] = (1 << (D - 1)) - r->coeffs[8 * i + 7];
  }
}

void smallpolyt1_unpack(smallpoly *r, const uint8_t *a)
{
  unsigned int i;

  for (i = 0; i < N / 4; ++i)
  {
    r->coeffs[4 * i + 0] = ((a[5 * i + 0] >> 0) | ((uint32_t)a[5 * i + 1] << 8)) & 0x3FF;
    r->coeffs[4 * i + 1] = ((a[5 * i + 1] >> 2) | ((uint32_t)a[5 * i + 2] << 6)) & 0x3FF;
    r->coeffs[4 * i + 2] = ((a[5 * i + 2] >> 4) | ((uint32_t)a[5 * i + 3] << 4)) & 0x3FF;
    r->coeffs[4 * i + 3] = ((a[5 * i + 3] >> 6) | ((uint32_t)a[5 * i + 4] << 2)) & 0x3FF;
  }
}

void smallpoly_challenge(smallpoly *c, const uint8_t seed[SEEDBYTES])
{
  unsigned int i, b, pos;
  uint64_t signs;
  uint8_t buf[SHAKE256_RATE];
  shake256incctx state;

  shake256_inc_init(&state);
  shake256_inc_absorb(&state, seed, SEEDBYTES);
  shake256_inc_finalize(&state);
  shake256_inc_squeezeblocks(buf, 1, &state);

  signs = 0;
  for (i = 0; i < 8; ++i)
    signs |= (uint64_t)buf[i] << 8 * i;
  pos = 8;

  for (i = 0; i < N; ++i)
    c->coeffs[i] = 0;
  for (i = N - TAU; i < N; ++i)
  {
    do
    {
      if (pos >= SHAKE256_RATE)
      {
        shake256_inc_squeezeblocks(buf, 1, &state);
        pos = 0;
      }

      b = buf[pos++];
    } while (b > i);

    c->coeffs[i] = c->coeffs[b];
    c->coeffs[b] = 1 - 2 * (signs & 1);
    signs >>= 1;
  }
}

/*
***********************************************************
** parallel small polynomial multiplication from ACSAC
***********************************************************
*/
#include"poly.h"
#include "params.h"


int32_t reduce32(int32_t a)
{
  int32_t t;

  t = (a + (1 << 22)) >> 23;
  t = a - t * Q;
  return t;
}

void smallpoly_getindex(uint8_t *cp_idx, const smallpoly *cp)
{
  unsigned int i=0,j=0;
  for(i=0;i<N;i++){
    if(cp->coeffs[i]!=0){
      cp_idx[j++]=(uint8_t)i;
    }
  }
}

// product scanning: would increase if instruction;
void poly_PSPM_PS(poly *r, const smallpoly *c, const smallpoly *s)
{
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i]=0;
    for (int j = 0; j <= i; j++)
    {
      if (c->coeffs[j] == 1)
      {
        r->coeffs[i] += s->coeffs[i-j]; // multiplication with 1 or -1
      }
      else if(c->coeffs[j]==-1)
      {
        r->coeffs[i] -= s->coeffs[i - j];
      }
    }
    for(int j=i+1;j<N;j++){
      if (c->coeffs[j] == 1)
      {
        r->coeffs[i] -= s->coeffs[i + N - j]; // multiplication with 1 or -1
      }
      else if (c->coeffs[j] == -1)
      {
        r->coeffs[i] += s->coeffs[i + N - j];
      }
    }
  }
}

// operand scanning
void poly_PSPM_OS(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s)
{
  int32_t w[2 * N] = {0};
  for (int i = 0; i < TAU; i++)
  {
    int k=c_idx[i];
    if (c->coeffs[k] == 1)
    {
      for (int j = 0; j < N; j++)
      {
        w[k + j] += s->coeffs[j]; // multiplication with 1 or -1
      }
    }
    else if (c->coeffs[k] == -1)
    {
      for (int j = 0; j < N; j++)
      {
        w[k + j] -= s->coeffs[j]; // multiplication with 1 or -1
      }
    }
  }
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i] = (w[i] - w[i + N]) % Q;
  }
}

extern void __asm_pspm_os(int32_t *r, int32_t *v, const int16_t *c, const uint8_t *c_idx, const int16_t *s);
void asm_poly_PSPM_OS(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s)
{
  int32_t w[2 * N] = {0};
  __asm_pspm_os(r->coeffs,w,c->coeffs,c_idx,s->coeffs);
}

// Product scanning
void poly_PSPM_precomp_PS(poly *r, const smallpoly *c, const smallpoly *s)
{
  int32_t v[2 * N];
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i]=0;
    v[i] = -s->coeffs[i];
    v[i+N]=s->coeffs[i];
  }
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (c->coeffs[j] != 0)
      {
        r->coeffs[i] += c->coeffs[j]*v[N+i-j]; // multiplication with 1 or -1
      }
    }
  }
}

// operand scanning Algo 6.
void poly_PSPM_precomp_OS(poly *r, const smallpoly *c, const smallpoly *s)
{
  int32_t v[2 * N];
  for (int i = 0; i < N; i++)
  {
    v[i] = -s->coeffs[i];
    v[i + N] = s->coeffs[i];
    r->coeffs[i] = 0;
  }
  for (int i = 0; i < N; i++)
  {
    if (c->coeffs[i] == 1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] += v[N + j - i]; // multiplication with 1 or -1
      }
    }
    else if (c->coeffs[i] == -1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] -= v[N + j - i]; // multiplication with 1 or -1
      }
    }
  }
}

// operand scanning algo 7
void poly_PSPM_precomp_OS_cs(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s)
{
  int32_t v[2 * N];
  for (int i = 0; i < N; i++)
  {
    v[i] = ETA - s->coeffs[i];
    v[i + N] = ETA + s->coeffs[i];
    r->coeffs[i] = 0;
  }
  for (int i = 0; i < TAU; i++)
  {
    int k=c_idx[i];
    if (c->coeffs[k] == 1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] += v[N + j - k]; // multiplication with 1 or -1
      }
    }
    else if (c->coeffs[k] == -1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] = r->coeffs[j] + 2 * ETA - v[N + j - k]; // multiplication with 1 or -1
      }
    }
  }
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i] = reduce32(r->coeffs[i] - TAU * ETA);
  }
}

void poly_PSPM_precomp_OS_ct0(poly *r, const smallpoly *c, const smallpoly *s)
{
  int32_t v[2 * N];
  for (int i = 0; i < N; i++)
  {
    v[i] = 4096 - s->coeffs[i];
    v[i + N] = 4096 + s->coeffs[i];
    r->coeffs[i] = 0;
  }
  for (int i = 0; i < N; i++)
  {
    if (c->coeffs[i] == 1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] += v[N + j - i]; // multiplication with 1 or -1
      }
    }
    else if (c->coeffs[i] == -1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] = r->coeffs[j] + 2 * 4096 - v[N + j - i]; // multiplication with 1 or -1
      }
    }
  }
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i] = reduce32(r->coeffs[i] - TAU * 4096);
  }
}

void poly_PSPM_precomp_OS_ct1(poly *r, const smallpoly *c, const smallpoly *s)
{
  int32_t v[2 * N];
  for (int i = 0; i < N; i++)
  {
    v[i] = 1024 - s->coeffs[i];
    v[i + N] = 1024 + s->coeffs[i];
    r->coeffs[i] = 0;
  }
  for (int i = 0; i < N; i++)
  {
    if (c->coeffs[i] == 1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] += v[N + j - i]; // multiplication with 1 or -1
      }
    }
    else if (c->coeffs[i] == -1)
    {
      for (int j = 0; j < N; j++)
      {
        r->coeffs[j] = r->coeffs[j] + 2 * 1024 - v[N + j - i]; // multiplication with 1 or -1
      }
    }
  }
  for (int i = 0; i < N; i++)
  {
    r->coeffs[i] = reduce32(r->coeffs[i] - TAU * 1024);
  }
}

void polyvecl_PSPM(polyvecl *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly s[L])
{
  unsigned int i;
  for (i = 0; i < L; i++)
  {
    poly_PSPM_precomp_OS_cs(&r->vec[i], c, c_idx, & s[i]);
  }
}

void polyveck_PSPM(polyveck *r, const smallpoly *c, const smallpoly s[K])
{
  unsigned int i;
  for (i = 0; i < K; i++)
  {
    poly_PSPM_precomp_OS_ct1(&r->vec[i], c, &s[i]);
  }
}