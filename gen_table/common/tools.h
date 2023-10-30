#ifndef TOOLS_H
#define TOOLS_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct compress_profile{
    size_t compressed_layers;
    size_t merged_layers[16];
};

// ================================
// reducing elements to mod mod_v

void cmod_int16(void *des, void *src, void *mod);
void cmod_int32(void *des, void *src, void *mod);
void cmod_int64(void *des, void *src, void *mod);

// ================================
// addition in mod mov_v

void addmod_int16(void *des, void *src1, void *src2, void *mod);
void addmod_int32(void *des, void *src1, void *src2, void *mod);

// ================================
// subtraction in mod mov_v

void submod_int16(void *des, void *src1, void *src2, void *mod);
void submod_int32(void *des, void *src1, void *src2, void *mod);

// ================================
// multiplication in mod mov_v

void mulmod_int16(void *des, void *src1, void *src2, void *mod);
void mulmod_int32(void *des, void *src1, void *src2, void *mod);

// ================================
// exponentiation in mod mov_v

void expmod_int16(void *des, void *src, size_t e, void *mod);
void expmod_int32(void *des, void *src, size_t e, void *mod);

// ================================
// in-place bit-reversing the array

void bitreverse(void *src, size_t len, size_t size);

#endif

