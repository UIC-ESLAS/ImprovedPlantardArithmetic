#ifndef GEN_TABLE_H
#define GEN_TABLE_H

#include <stddef.h>
#include <stdbool.h>

#include "tools.h"

// ================================
// generate twiddle factors for cyclic NTT with Cooley-Tukey butterflies

void gen_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

// ================================
// generate twiddle factors for negacyclic NTT with Cooley-Tukey butterflies

void gen_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

// ================================
// generate twiddle factors for cyclic iNTT with Cooley-Tukey butterflies

void gen_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t _size, void *_mod)
    );

// ================================
// generate twiddle factors for cyclic NTT with Cooley-Tukey butterflies
// the table is re-ordered according to _profile

void gen_streamlined_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    );

// ================================
// generate twiddle factors for negacyclic NTT with Cooley-Tukey butterflies
// the table is re-ordered according to _profile

void gen_streamlined_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    );

// ================================
// generate twiddle factors for cyclic iNTT with Cooley-Tukey butterflies
// the table is re-ordered according to _profile

void gen_streamlined_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    );

// ================================
// generate twiddle factors for twisting (x^NTT_N - omega^NTT_N) to (x^NTT_N - 1)

void gen_twist_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

// ================================
// generate twiddle factors for negacyclic iNTT with Cooley-Tukey butterflies
// the table is re-ordered according to _profile

void gen_streamlined_inv_CT_negacyclic_table_generic(
    void *des,
    void *scale1, void *omega,
    void *scale2, void *twist_omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    );

// ================================
// generate twiddle factors for multiplication in
// x^(ARRAY_N / NTT_N) +- omega^i

void gen_mul_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *mod)
    );

#endif

