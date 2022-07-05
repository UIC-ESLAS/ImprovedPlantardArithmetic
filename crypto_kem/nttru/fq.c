#include <stdint.h>
#include "params.h"
#include "randombytes.h"
#include "fq.h"

int16_t fqmontred(int32_t a) {
  int32_t t;
  int16_t u;

  u = a * QINV;
  t = (int32_t)u * Q;
  t = a - t;
  t >>= 16;
  return t;
}

int16_t fqplantred(int32_t a){
  int32_t t;
  t=(int32_t)a*QINV_PLANT;
  t>>=16;
  t=(t+4)*Q;//4=2^\alpha; \alpha=2 for NTTRU
  t>>=16;
  return t;
}


int16_t fqred16(int16_t a) {
  int16_t t;

  t = a & 0x1FFF;
  a >>= 13;
  t += (a << 9) - a;
  return t;
}

int16_t fqcsubq(int16_t a) {
  a += (a >> 15) & Q;
  a -= Q;
  a += (a >> 15) & Q;
  return a;
}

int16_t fqmul(int16_t a, int16_t b) {
  return fqmontred((int32_t)a*b);
}

int16_t fqmul_plant(int16_t a, int16_t b){
  return fqplantred((int32_t)a * b);
}

int16_t fqmul_plant_const(int16_t a, int32_t b)
{
  int32_t t;
  t = (int32_t)a * b;
  t >>= 16;
  t = (t + 4) * Q; // 4=2^\alpha; \alpha=2 for NTTRU
  t >>= 16;
  return t;
}

// a^{q-2}=a^7679=a^1110111111111b
int16_t fqinv(int16_t a)
{
  unsigned int i;
  int16_t t;

  t = a;
  for(i = 1; i <= 12; ++i) {
    a = fqmul(a, a);//a=a^2 a^4
    if(i != 9) t = fqmul(t, a);//t=a^3 a^7
  }

  return t;
}

int16_t fqinv_plant(int16_t a)
{
  unsigned int i;
  int16_t t;

  t = a;
  for (i = 1; i <= 12; ++i)
  {
    a = fqmul_plant(a, a); // a=a^2 a^4
    if (i != 9)
      t = fqmul_plant(t, a); // t=a^3 a^7
  }

  return t;
}

int16_t fquniform(void) {
  int16_t r;

  do {
    randombytes((unsigned char*) &r, 2);
    r &= 0x1FFF;
  } while(r >= Q);

  return r;
}
