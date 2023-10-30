#include "NTT_params.h"

#include "tools.h"
#include "ntt_c.h"

void CT_butterfly_int16(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    ){

    CT_butterfly_generic(
        src,
        indx_a, indx_b,
        twiddle,
        mod,
        size,
        addmod_int16,
        submod_int16,
        mulmod_int16
        );

}

void CT_butterfly_int32(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    ){

    CT_butterfly_generic(
        src,
        indx_a, indx_b,
        twiddle,
        mod,
        size,
        addmod_int32,
        submod_int32,
        mulmod_int32
        );

}

void m_layer_CT_butterfly_int16(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    ){

    m_layer_CT_butterfly_generic(
        src,
        layers, step,
        _root_table,
        mod,
        size,
        CT_butterfly_int16
        );

}

void m_layer_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    ){

    m_layer_CT_butterfly_generic(
        src,
        layers, step,
        _root_table,
        mod,
        size,
        CT_butterfly_int32
        );

}

void m_layer_inv_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    ){

    m_layer_inv_CT_butterfly_generic(
        src,
        layers, step,
        _root_table,
        mod,
        size,
        CT_butterfly_int32
        );

}

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
    ){

    char tmp[size];

    mulmod(tmp, src + indx_b * size, twiddle, mod);
    submod(src + indx_b * size, src + indx_a * size, tmp, mod);
    addmod(src + indx_a * size, src + indx_a * size, tmp, mod);

}

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
    ){

    size_t real_count, real_step, twiddle_count, jump;
    void *real_root_table;

    for(size_t i = 0; i < layers; i++){

        twiddle_count = 1 << i;

        real_count = 1 << (layers - 1 - i);

        jump = step << (layers - i);

        real_root_table = _root_table + ((1 << i) - 1) * size;

        real_step = step << (layers - 1 - i);

        for(size_t k = 0; k < real_count; k++){
            for(size_t j = 0; j < twiddle_count; j++){
                CT_butterfly(
                    src + (j * jump + k * step) * size,
                    0, real_step,
                    real_root_table + j * size,
                    mod,
                    size
                    );
            }
        }

    }

}

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
    ){

    size_t real_count, real_step, twiddle_count, jump;
    void *real_root_table;

    for(size_t i = 0; i < layers; i++){

        twiddle_count = 1 << i;

        real_count = 1 << (layers - 1 - i);

        jump = step << (i + 1);

        real_root_table = _root_table + ((1 << i) - 1) * size;

        real_step = step << i;

        for(size_t k = 0; k < real_count; k++){
            for(size_t j = 0; j < twiddle_count; j++){
                CT_butterfly(
                    src + (j * step + k * jump) * size,
                    0, real_step,
                    real_root_table + j * size,
                    mod,
                    size
                    );
            }
        }

    }

}

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
    ){

    size_t step, offset;
    size_t real_start_level, real_end_level;
    void *real_root_table;
    size_t *level_indx;

    real_start_level = 0;
    for(size_t i = 0; i < start_level; i++){
        real_start_level += (_profile->merged_layers)[i];
    }

    real_end_level = real_start_level;
    for(size_t i = start_level; i < end_level; i++){
        real_end_level += (_profile->merged_layers)[i];
    }

    real_root_table = _root_table + ((1 << real_start_level) - 1) * size;

    level_indx = (_profile->merged_layers) + start_level;

    for(size_t level = real_start_level; level <= real_end_level; level += *(level_indx++)){

        step = ARRAY_N >> (level + (*level_indx));

        offset = 0;

        for(size_t count = 0; count < (1 << level); count++){

            for(size_t i = 0; i < step; i++){
                m_layer_CT_butterfly(
                    src + (offset + i) * size,
                    *level_indx, step,
                    real_root_table,
                    mod,
                    size
                    );
            }

            offset += ARRAY_N >> level;

            real_root_table += ((1 << (*level_indx)) - 1) * size;

        }

    }

}

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
    ){

    size_t step, offset;
    size_t real_start_level, real_end_level;
    void *real_root_table;
    size_t *level_indx;

    real_start_level = 0;
    for(size_t i = 0; i < start_level; i++){
        real_start_level += (_profile->merged_layers)[i];
    }

    real_end_level = real_start_level;
    for(size_t i = start_level; i < end_level; i++){
        real_end_level += (_profile->merged_layers)[i];
    }

    real_root_table = _root_table + ((1 << real_start_level) - 1) * size;

    level_indx = (_profile->merged_layers) + start_level;

    for(size_t level = real_start_level; level <= real_end_level; level += *(level_indx++)){

        step = (ARRAY_N >> LOGNTT_N) << level;

        for(size_t count = 0; count < (1 << level); count++){

            offset = count * (ARRAY_N >> LOGNTT_N);

            for(size_t i = 0; i < (NTT_N >> ((*level_indx) + level)); i++){

                for(size_t j = 0; j < (ARRAY_N >> LOGNTT_N); j++){
                    m_layer_inv_CT_butterfly(
                        src + (offset + j) * size,
                        *level_indx, step,
                        real_root_table,
                        mod,
                        size
                        );
                }

                offset += (ARRAY_N >> LOGNTT_N) << ((*level_indx) + level);

            }

            real_root_table += ((1 << (*level_indx)) - 1) * size;

        }

    }

}












