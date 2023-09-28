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
#include "implvariant.h"
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void)
{

    unsigned long long t0, t1;
    hal_setup(CLOCK_BENCHMARK);
    poly buf,c1;
    polyvecl s1;
    polyveck s2;
    int16_t a[N];
    int16_t b[N];
    int16_t c[N];
    int16_t b_prime[N];

    shake256incctx state;

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

        // ### SHAKE256 ###
        t0 = hal_get_time();
        KeccakF1600_StatePermute(&state);
        t1 = hal_get_time();
        printcycles("KeccakF1600_StatePermute cycles:", t1 - t0);

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
        printcycles("small basemul cycles:", t1 - t0);

        // cs1
        poly_challenge(&c1, (uint8_t *)"sig");
        t0 = hal_get_time();
        polyvecl_ntt(&s1);
        poly_ntt(&c1);
        polyvecl_pointwise_poly_montgomery(&s1, &c1, &s1);
        polyvecl_invntt_tomont(&s1);
        t1 = hal_get_time();
        printcycles("cs1 with 32-bit NTT cycles:", t1 - t0);

        // cs2
        poly_challenge(&c1, (uint8_t *)"sig");
        t0 = hal_get_time();
        polyveck_ntt(&s2);
        poly_ntt(&c1);
        polyveck_pointwise_poly_montgomery(&s2, &c1, &s2);
        polyveck_invntt_tomont(&s2);
        t1 = hal_get_time();
        printcycles("cs2 with 32-bit NTT cycles:", t1 - t0);

        // cs1 with 16-bit NTT
        smallpoly ss1[L],ss2[K],sc1;
#ifdef old
        smallhalfpoly scprime;
#else
        smallpoly scprime;
#endif
        poly_challenge(&c1, (uint8_t *)"sig");
        t0 = hal_get_time();
        poly_small_ntt_precomp(&sc1, &scprime, &c1);
        polyvecl_small_ntt(ss1);
        for(int i=0;i<L;i++){
            
            poly_small_basemul_invntt(&s1.vec[i], &sc1, &scprime, &ss1[i]);
        }
        t1 = hal_get_time();
        printcycles("cs1 with 16-bit NTT cycles:", t1 - t0);

        // cs2
        poly_challenge(&c1, (uint8_t *)"sig");
        t0 = hal_get_time();
        poly_small_ntt_precomp(&sc1, &scprime, &c1);
        polyveck_small_ntt(ss2);
        for(int i=0;i<K;i++){
            poly_small_basemul_invntt(&s1.vec[i], &sc1, &scprime, &ss2[i]);
        }
        t1 = hal_get_time();
        printcycles("cs2 with 16-bit NTT cycles:", t1 - t0);

        hal_send_str("#");
    }

    while (1)
        ;
    return 0;
}
