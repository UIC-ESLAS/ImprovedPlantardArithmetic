#ifndef NTT_H
#define NTT_H

#include <stdint.h>


extern const int32_t zetas[128];

// void init_ntt();
void ntt(int16_t a[768]);
void invntt(int16_t a[768]);
void ntt_pack(int16_t b[768], const int16_t a[768]);
void ntt_unpack(int16_t b[768], const int16_t a[768]);
#endif
