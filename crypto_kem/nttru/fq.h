#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

#define MONT 4088 // 2^16 % Q
#define QINV 57857 // q^(-1) mod 2^16
#define PLANT 2112
#define QINV_PLANT 2340676097 // q^(-1) mod 2^32
int16_t fqmontred(int32_t a);
int16_t fqred16(int16_t a);
int16_t fqcsubq(int16_t a);
int16_t fqmul(int16_t a, int16_t b);
int16_t fqmul_plant(int16_t a, int16_t b);
int16_t fqmul_plant_const(int16_t a, int32_t b);
int16_t	fqinv(int16_t a);
int16_t fqinv_plant(int16_t a);
int16_t fquniform(void);

#endif
