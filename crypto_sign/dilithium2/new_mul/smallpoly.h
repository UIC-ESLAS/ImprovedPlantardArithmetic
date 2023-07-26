#ifndef SMALLPOLY_H
#define SMALLPOLY_H
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include <stdint.h>

typedef struct {
    int16_t coeffs[N];
} smallpoly;

void small_polyeta_unpack(smallpoly *r, const uint8_t *a);
void smallpolyt1_unpack(smallpoly *r, const uint8_t *a);
void smallpolyt0_unpack(smallpoly *r, const uint8_t *a);
void smallpoly_challenge(smallpoly *c, const uint8_t seed[SEEDBYTES]);
/*
***********************************************************
** parallel small polynomial multiplication from ACSAC
***********************************************************
*/
void smallpoly_getindex(uint8_t *cp_idx, const smallpoly *cp);
void poly_PSPM_OS(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s);
void poly_PSPM_PS(poly *r, const smallpoly *c, const smallpoly *s);
void asm_poly_PSPM_OS(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s);
void poly_PSPM_precomp_PS(poly *r, const smallpoly *c, const smallpoly *s);
void poly_PSPM_precomp_OS(poly *r, const smallpoly *c, const smallpoly *s);
void poly_PSPM_precomp_OS_cs(poly *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly *s);
void poly_PSPM_precomp_OS_ct0(poly *r, const smallpoly *c, const smallpoly *s);
void poly_PSPM_precomp_OS_ct1(poly *r, const smallpoly *c, const smallpoly *s);
void polyvecl_PSPM(polyvecl *r, const smallpoly *c, const uint8_t *c_idx, const smallpoly s[L]);
void polyveck_PSPM(polyveck *r, const smallpoly *c, const smallpoly s[K]);

#endif