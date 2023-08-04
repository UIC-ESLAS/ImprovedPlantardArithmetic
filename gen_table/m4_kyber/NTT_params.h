#ifndef NTT_PARAMS_H
#define NTT_PARAMS_H

#define ARRAY_N 256

#define NTT_N 128
#define LOGNTT_N 7

#define Q1 3329
#define Q1pr 13
// omegaQ1 = Q1pr^((Q1 - 1) / (NTT_N << 1)) mod Q1
#define omegaQ1 17
// invomegaQ1 = omegaQ1^{-1} mod Q1
#define invomegaQ1 1175
// RmodQ1 = 2^16 mod^{+-} Q1
#define RmodQ1 -1044
// R2modQ1=2^32 mod^{+-} Q1 for Plantard arithmetic
#define R2modQ1 1976
// Q1prime = -Q1^{-1} mod^{+-} 2^16
#define Q1prime 3327

// Q1prime2 = Q1^{-1} mod^{+-} 2^32 for Plantard arithmetic
#define Q1prime2 1806234369 

// invNQ1 = NTT_N^{-1} mod Q1
#define invNQ1 3303

#endif

