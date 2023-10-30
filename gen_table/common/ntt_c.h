#ifndef NTT_C_H
#define NTT_C_H

#include "tools.h"


void CT_butterfly_int16(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    );

void CT_butterfly_int32(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    );

void m_layer_CT_butterfly_int16(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    );

void m_layer_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    );

void m_layer_inv_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    );

// ================================
// Cooley-Tukey butterfly

void CT_butterfly_generic(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *des, void *src1, void *src2, void *mod),
    void (*submod)(void *des, void *src1, void *src2, void *mod),
    void (*mulmod)(void *des, void *src1, void *src2, void *mod)
    );

// ================================
// multi-layer Cooley-Tukey butterfly for the forward transformation

void m_layer_CT_butterfly_generic(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size,
    void (*CT_butterfly)(
        void *src,
        size_t indx_a, size_t indx_b,
        void *twiddle,
        void *mod,
        size_t size
        )
    );

// ================================
// multi-layer Cooley-Tukey butterfly for the inverse transformation

void m_layer_inv_CT_butterfly_generic(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size,
    void (*CT_butterfly)(
        void *src,
        size_t indx_a, size_t indx_b,
        void *twiddle,
        void *mod,
        size_t size
        )
    );

// ================================
// NTT with Cooley-Tukey butterfly
// we must use m_layer_CT_butterfly here

void compressed_CT_NTT_generic(
    void *src,
    size_t start_level, size_t end_level,
    void *_root_table,
    void *mod,
    struct compress_profile *_profile,
    size_t size,
    void (*m_layer_CT_butterfly)(
        void *src,
        size_t layers, size_t step,
        void *_root_table,
        void *mod,
        size_t size
        )
    );

// ================================
// iNTT with Cooley-Tukey butterfly
// we must use m_layer_inv_CT_butterfly here

void compressed_inv_CT_NTT_generic(
    void *src,
    size_t start_level, size_t end_level,
    void *_root_table,
    void *mod,
    struct compress_profile *_profile,
    size_t size,
    void (*m_layer_inv_CT_butterfly)(
        void *src,
        size_t layers, size_t step,
        void *_root_table,
        void *mod,
        size_t size
        )
    );


#endif

