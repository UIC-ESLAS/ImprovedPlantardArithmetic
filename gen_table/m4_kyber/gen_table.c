
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"

void gen_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < (NTT_N >> 1); i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

    des -= size * (NTT_N >> 1);
    bitreverse(des, NTT_N >> 1, size);

}

void gen_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    char zeta[size];

    memcpy(zeta, omega, size);

    memcpy(des, scale, size);
    for(size_t i = 1; i < NTT_N; i++){
        mulmod(des + i * size, des + (i - 1) * size, zeta, mod);
    }

    bitreverse(des, NTT_N, size);

    for(size_t i = 1; i < NTT_N; i++){
        memcpy(des + (i - 1) * size, des + i * size, size);
    }

}

uint16_t tree[128] = {
    0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
    4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
    2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
    6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
    1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
    5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
    3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
    7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127};

// generate correct GS table with the final two twiddle factor multipliying with R^2 N^-1.
void gen_inv_GS_negacyclic_table_generic(
    void *res, void* des,
    void *scale, void *omega,
    void *scale2,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod))
{
    char zeta[size];
    char zero[size];
    
    memset(zero, 0, size);
    memcpy(zeta, omega, size);

    memcpy(des, scale, size); // des[0]=MONT
    for (size_t i = 1; i < NTT_N; i++)
    {
        mulmod(des + i * size, des + (i - 1) * size, zeta, mod);
    }

    unsigned int i,j,k;
    k = 0;
    for (i = 64; i >= 1; i >>= 1){
        for (j = i; j < 2 * i; ++j)
        {
            memcpy(res+k*size, (des + (128-tree[j]) * size), size);
            submod_int16(res+k*size, zero, res+k*size, mod);
            k++;
        }
    }
    mulmod(res+126*size, res+126*size, scale2, mod);
    mulmod(scale2, scale2, scale, mod);
    memcpy(res+127*size, scale2, size); // final one
}

void gen_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t _size, void *_mod)
    ){
    char zeta[size];
    char twiddle[size];

    for(size_t level = 0; level < LOGNTT_N; level++){
        expmod(zeta, omega, (1 << LOGNTT_N) >> (level + 1), mod);
        memcpy(twiddle, scale, size);
        for(size_t i = 0; i < (1 << level); i++){
            memcpy(des, twiddle, size);
            des += size;
            mulmod(twiddle, twiddle, zeta, mod);
        }
    }

}


void gen_streamlined_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_CT_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp;
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(des,
                        level_ptr[start_level + k] + (j * (1 << k) + h) * size,
                        size);
                    des += size;
                }
            }
        }
    start_level += (_profile->merged_layers)[i];
    }

}

// Kyber CT NTT
void gen_streamlined_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_CT_negacyclic_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(des,
                        level_ptr[start_level + k] + (j * (1 << k) + h) * size,
                        size);
                    des += size;
                }
            }
        }
    start_level += (_profile->merged_layers)[i];
    }

}

// Kyber CT NTT Plantard
void gen_streamlined_CT_negacyclic_table_generic_plant(
    void *des,
    void *scale, void *omega,
    void *mod,
    void *mod_inv,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*mul)(void *_des, void *_src1, void *_src2),
    struct compress_profile *_profile, bool pad)
{

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_CT_negacyclic_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod);

    for (size_t i = 0; i < LOGNTT_N; i++)
    {
    level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    start_level = 0;
    for (size_t i = 0; i < _profile->compressed_layers; i++)
    {
    for (size_t j = 0; j < (1 << start_level); j++)
    {
            if (pad)
            {
                memset(des, 0, 2*size);
                des += 2*size;
            }
            for (size_t k = 0; k < (_profile->merged_layers[i]); k++)
            {
                for (size_t h = 0; h < (1 << k); h++)
                {
                    memcpy(zeta,
                           level_ptr[start_level + k] + (j * (1 << k) + h) * size,
                           size);
                    mul(des,zeta,mod_inv);
                    des += 2*size;
                }
            }
    }
    start_level += (_profile->merged_layers)[i];
    }
}
// geninv GS butterfly Plantard
void gen_streamlined_inv_GS_negacyclic_table_generic_plant(
    void *des,
    void *scale, void *omega,
    void *scale2,
    void *mod,
    void *mod_inv,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*mul)(void *_des, void *_src1, void *_src2),
    struct compress_profile *_profile, bool pad)
{

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    char tmp1[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_inv_GS_negacyclic_table_generic(
        tmp, tmp1, scale, zeta, scale2, mod,
        size,
        mulmod);

    printf("\n leverl_ptr:");
    for (int i = LOGNTT_N-1; i >= 0; i--)
    {
        size_t j=LOGNTT_N-i;
        printf("%d ", (128 - (1 << j)));
        level_ptr[i] = tmp + size * (128-(1 << j));
    }
    printf("\n");

    start_level = 0;
    for (size_t i = 0; i < _profile->compressed_layers; i++)
    {
        int num_j = 1 << (LOGNTT_N - start_level-_profile->merged_layers[i]);// maximum index of j
        for (size_t j = 0; j < num_j; j++)
        {
            if (pad)
            {
                memset(des, 0, 2*size);
                des += 2*size;
            }
            for (int k = (_profile->merged_layers[i]-1); k>=0; k--)
            {
                for (size_t h = 0; h < (1 << k); h++)
                {
                    memcpy(zeta,
                           level_ptr[start_level + (_profile->merged_layers[i] - k - 1)] + (j * (1 << k) + h) * size,
                           size);
                    mul(des,zeta,mod_inv);
                    des += 2*size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }
    memcpy(zeta,tmp+127*size,size);
    mul(des, zeta, mod_inv);
}

// geninv GS butterfly 
void gen_streamlined_inv_GS_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *scale2,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad)
{

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    char tmp1[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_inv_GS_negacyclic_table_generic(
        tmp, tmp1, scale, zeta, scale2, mod,
        size,
        mulmod);

    // printf("\n leverl_ptr:");
    for (int i = LOGNTT_N - 1; i >= 0; i--)
    {
        size_t j = LOGNTT_N - i;
        // printf("%d ", (128 - (1 << j)));
        level_ptr[i] = tmp + size * (128 - (1 << j));
    }
    // printf("\n");

    start_level = 0;
    for (size_t i = 0; i < _profile->compressed_layers; i++)
    {
        int num_j = 1 << (LOGNTT_N - start_level - _profile->merged_layers[i]); // maximum index of j
        for (size_t j = 0; j < num_j; j++)
        {
            if (pad)
            {
                memset(des, 0, size);
                des += size;
            }
            for (int k = (_profile->merged_layers[i] - 1); k >= 0; k--)
            {
                for (size_t h = 0; h < (1 << k); h++)
                {
                    memcpy(des,
                           level_ptr[start_level + (_profile->merged_layers[i] - k - 1)] + (j * (1 << k) + h) * size,
                           size);
                    des += size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }
    memcpy(des, tmp + 127 * size, size);
}

void gen_twist_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){
    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < NTT_N; i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

}

void gen_streamlined_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];
    size_t start_level;

    char tmp[NTT_N * size];

    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_inv_CT_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod,
        expmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(
                        des,
                        level_ptr[start_level + k] + (j + (h << start_level)) * size,
                        size);
                    des += size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}

// Kyber CT INTT
void gen_streamlined_inv_CT_negacyclic_table_generic(
    void *des,
    void *scale1, void *omega,
    void *scale2, void *twist_omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];
    size_t start_level;

    char tmp[NTT_N * size];
    char tmp2[NTT_N * size];

    void *level_ptr[LOGNTT_N];
    char *twist_ptr;

    memcpy(zeta, omega, size);

    gen_inv_CT_table_generic(
        tmp, scale1, zeta, mod,
        size,
        mulmod,
        expmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    memcpy(zeta, twist_omega, size);

    gen_twist_table_generic(
        tmp2, scale2, zeta, mod,
        size,
        mulmod
    );

    twist_ptr = tmp2;

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(
                        des,
                        level_ptr[start_level + k] + (j + (h << start_level)) * size,
                        size);
                    des += size;
                }
            }
            if((i + 1) == _profile->compressed_layers){
                for(size_t k = 0; k < (1 << (_profile->merged_layers[i])); k++){
                    memcpy(
                        des,
                        twist_ptr + (j + k * (NTT_N >> (_profile->merged_layers[i]))) * size,
                        size);
                    des += size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}

// Kyber CT INTT Plantard
void gen_streamlined_inv_CT_negacyclic_table_generic_plant(
    void *des,
    void *scale1, void *omega,
    void *scale2, void *twist_omega,
    void *mod,
    void *mod_inv,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod)(void *_des, void *_src, size_t e, void *_mod),
    void (*mul)(void *_des, void *_src1, void *_src2),
    struct compress_profile *_profile, bool pad)
{
    char zeta[size];
    size_t start_level;

    char tmp[NTT_N * size];
    char tmp2[NTT_N * size];

    void *level_ptr[LOGNTT_N];
    char *twist_ptr;

    memcpy(zeta, omega, size);

    gen_inv_CT_table_generic(
        tmp, scale1, zeta, mod,
        size,
        mulmod,
        expmod);

    for (size_t i = 0; i < LOGNTT_N; i++)
    {
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    memcpy(zeta, twist_omega, size);

    gen_twist_table_generic(
        tmp2, scale2, zeta, mod,
        size,
        mulmod);

    twist_ptr = tmp2;

    start_level = 0;
    for (size_t i = 0; i < _profile->compressed_layers; i++)
    {
        for (size_t j = 0; j < (1 << start_level); j++)
        {
            if (pad)
            {
                memset(des, 0, 2*size);
                des += 2 * size;
            }
            for (size_t k = 0; k < (_profile->merged_layers[i]); k++)
            {
                for (size_t h = 0; h < (1 << k); h++)
                {
                    memcpy(
                        zeta,
                        level_ptr[start_level + k] + (j + (h << start_level)) * size,
                        size);
                    mul(des, zeta, mod_inv);
                    des += 2 * size;
                }
            }
            if ((i + 1) == _profile->compressed_layers)
            {
                for (size_t k = 0; k < (1 << (_profile->merged_layers[i])); k++)
                {
                    memcpy(
                        zeta,
                        twist_ptr + (j + k * (NTT_N >> (_profile->merged_layers[i]))) * size,
                        size);
                    mul(des, zeta, mod_inv);
                    des += 2 * size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }
}

void gen_mul_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *mod)
    ){

    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < (NTT_N >> 1); i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

    des -= (NTT_N >> 1) * size;

    bitreverse(des, NTT_N >> 1, size);

}









