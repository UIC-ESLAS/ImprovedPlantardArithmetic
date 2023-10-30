#ifndef NAIVE_MULT
#define NAIVE_MULT

#include <stddef.h>

// ================================
// multiplication in R[x] / (x^len - twiddle)
// where R is the ring defined by mod, addmod, mulmod

void naive_mulR(
    void *des,
    void *src1, void *src2,
    size_t len, void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

// ================================
// point-wise multiplication of src1[len * jump] by src2[len]
// in particular, for i in {0, ..., len} and j in {0, ..., jump},
// src1[i * jump + j] is multiplied by src2[i]

void point_mul(
    void *des,
    void *src1, void *src2,
    size_t len, size_t jump,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

#endif

