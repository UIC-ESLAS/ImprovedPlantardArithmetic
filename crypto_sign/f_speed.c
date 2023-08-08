#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "smallntt.h"


#include "params.h"
#include "symmetric.h"
#include "sign.h"
#include "poly.h"
#include "polyvec.h"
#include "ntt.h"
#include "smallpoly.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void)
{

    unsigned long long t0, t1;
    hal_setup(CLOCK_BENCHMARK);
    poly buf,a1,c1;
#ifdef optPSPM
    polyveck ak, ck;
    polyvecl al, cl;
    smallpoly bs;
    uint8_t idx[TAU];
#elif defined(opt)
    int16_t a[N];
    int16_t b[N];
    int16_t c[N];
    int16_t b_prime[N];
#endif
    shake256incctx state;
    uint8_t seedbuf[SEEDBYTES];

    shake256_inc_init(&state);
    
    
    for (int i = 0; i < 2; i++)
    {
        hal_send_str("==========================");
    }

    for (int crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // ### NTT ###
        t0 = hal_get_time();
        poly_ntt(&buf);
        t1 = hal_get_time();
        printcycles("NTT cycles:", t1 - t0);
        
        // ### iNTT ###
        t0 = hal_get_time();
        poly_invntt_tomont(&buf);
        t1 = hal_get_time();
        printcycles("iNTT cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_pointwise_montgomery(&buf, &buf, &buf);
        t1 = hal_get_time();
        printcycles("basemul cycles:", t1 - t0);
        
        // ### multi-moduli NTT ###
        t0 = hal_get_time();
        poly_double_ntt(&a1);
        t1 = hal_get_time();
        printcycles("Multi-moduli NTT cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_double_ntt_precomp(&c1, &buf);
        t1 = hal_get_time();
        printcycles("Multi-moduli NTT precomp cycles:", t1 - t0);

        // ### iNTT ###
        t0 = hal_get_time();
        poly_double_basemul_invntt(&a1, &buf, &c1, &a1);
        t1 = hal_get_time();
        printcycles("Multi-moduli basemul+iNTT cycles:", t1 - t0);

        // ### SHAKE256 ###
        t0 = hal_get_time();
        KeccakF1600_StatePermute(&state);
        t1 = hal_get_time();
        printcycles("SHAKE256 absorb cycles:", t1 - t0);

        shake256_inc_finalize(&state);
        
        t0 = hal_get_time();
        KeccakF1600_StatePermute(&state);
        t1 = hal_get_time();
        printcycles("SHAKE256 squeeze cycles:", t1 - t0);
#ifdef opt
        // ### small NTT ###
        t0 = hal_get_time();
        small_ntt(a);
        t1 = hal_get_time();
        printcycles("small NTT cycles:", t1 - t0);

        t0 = hal_get_time();
        small_invntt_tomont(a);
        t1 = hal_get_time();
        printcycles("small iNTT cycles:", t1 - t0);

        t0 = hal_get_time();
        small_point_mul(b_prime, b);
        t1 = hal_get_time();
        printcycles("small point_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        small_asymmetric_mul(c, a, b, b_prime);
        t1 = hal_get_time();
        printcycles("small asymmetric_mul cycles:", t1 - t0);
#elif defined(optPSPM)
        // ### PSPM ###
        smallpoly_challenge(&bs, "abc");
        smallpoly_getindex(idx,&bs);
        t0 = hal_get_time();
        polyvecl_PSPM(&al, &bs, idx, & cl);
        t1 = hal_get_time();
        printcycles("polyvecl_PSPM cycles:", t1 - t0);

        t0 = hal_get_time();
        polyveck_PSPM(&ak, &bs, &ck);
        t1 = hal_get_time();
        printcycles("polyveck_PSPM cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_OS(&a1,&bs,idx,&c1);
        t1 = hal_get_time();
        printcycles("PSPM_OS cycles:", t1 - t0);

        t0 = hal_get_time();
        asm_poly_PSPM_OS(&a1, &bs, idx, &c1);
        t1 = hal_get_time();
        printcycles("asm_PSPM_OS cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_PS(&a1, &bs, &c1);
        t1 = hal_get_time();
        printcycles("PSPM_PS cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_precomp_OS(&a1, &bs, &c1);
        t1 = hal_get_time();
        printcycles("PSPM_precomp_OS cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_precomp_OS_cs(&a1, &bs, idx, & c1);
        t1 = hal_get_time();
        printcycles("PSPM_precomp_OS_cs cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_precomp_OS_ct0(&a1, &bs, &c1);
        t1 = hal_get_time();
        printcycles("PSPM_precomp_OS_ct0 cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_precomp_OS_ct1(&a1, &bs, &c1);
        t1 = hal_get_time();
        printcycles("PSPM_precomp_OS_ct1 cycles:", t1 - t0);

        t0 = hal_get_time();
        poly_PSPM_precomp_PS(&a1, &bs, &c1);
        t1 = hal_get_time();
        printcycles("PSPM_precomp_PS cycles:", t1 - t0);
#endif
        hal_send_str("OK KEYS\n");

        hal_send_str("#");
    }

    while (1)
        ;
    return 0;
}
