#include "ntt.h"

#include "params.h"

#include <stdint.h>

/* Code to generate zetas and zetas_inv used in the number-theoretic transform:

#define KYBER_ROOT_OF_UNITY 17

static const uint16_t tree[128] = {
  0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
  4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
  2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
  6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
  1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
  5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
  3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
  7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127};


static int16_t fqmul(int16_t a, int16_t b) {
  return montgomery_reduce((int32_t)a*b);
}

void init_ntt() {
  unsigned int i, j, k;
  int16_t tmp[128];

  tmp[0] = MONT;
  for(i = 1; i < 128; ++i)
    tmp[i] = fqmul(tmp[i-1], KYBER_ROOT_OF_UNITY*MONT % KYBER_Q);

  for(i = 0; i < 128; ++i)
    zetas[i] = tmp[tree[i]];

  k = 0;
  for(i = 64; i >= 1; i >>= 1)
    for(j = i; j < 2*i; ++j)
      zetas_inv[k++] = -tmp[128 - tree[j]];

  zetas_inv[127] = MONT * (MONT * (KYBER_Q - 1) * ((KYBER_Q - 1)/128) % KYBER_Q) % KYBER_Q;
}

*/

// Using word-size modular arithmetic in "Efficient Word Size Modular Arithmetic"
const int32_t zetas[64] = { 21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026, 1419184148, 1817845876, 3434425636, 4233039261, 300609006, 975366560, 2781600929, 3889854731, 3935010590, 2197155094, 2130066389, 3598276897, 2308109491, 2382939200, 1228239371, 1884934581, 3466679822, 1211467195, 2977706375, 3144137970, 3080919767, 945692709, 3015121229, 345764865, 826997308, 2043625172, 2964804700, 2628071007, 4154339049, 483812778, 3288636719, 2696449880, 2122325384, 1371447954, 411563403, 3577634219, 976656727, 2708061387, 723783916, 3181552825, 3346694253, 3617629408, 1408862808, 519937465, 1323711759, 1474661346, 2773859924, 3580214553, 1143088323, 2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155, 2779020594 };

const int32_t zetas_asm[128] = {
  // 7 & 6 & 5 layers
  2230699446, 3328631909, 4243360600, 3408622288, 812805467, 2447447570, 1094061961, 
  // 1st loop of 4 & 3 & 2 layers
  1370157786, 381889553, 3157039644, 372858381, 427045412, 4196914574, 2265533966, 
  // 2nd loop of 4 & 3 & 2 layers
  2475831253, 1727534158, 1904287092, 1544330386, 2972545705, 2937711185, 2651294021, 
  // 3rd loop of 4 & 3 & 2 layers
  249002310, 3929849920, 72249375, 838608815, 2550660963, 3242190693, 815385801, 
  // 4th loop of 4 & 3 & 2 layers
  1028263423, 2889974991, 1719793153, 3696329620, 42575525, 1703020977, 2470670584, 
  // 5th loop of 4 & 3 & 2 layers
  3594406395, 1839778722, 2701610550, 2991898216, 1851390229, 1041165097, 583155668, 
  // 6th loop of 4 & 3 & 2 layers
  4205945745, 690239563, 3718262466, 1855260731, 3700200122, 1979116802, 3098982111, 
  // 7th loop of 4 & 3 & 2 layers
  734105255, 3087370604, 3714391964, 3415073125, 3376368103, 1910737929, 836028480, 
  // 8th loop of 4 & 3 & 2 layers
  2252632292, 2546790461, 1059227441, 3191874164, 4012420634, 1583035408, 1174052340, 
  // 1 layer
  21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026, 1419184148, 1817845876, 3434425636, 4233039261, 300609006, 975366560, 2781600929, 3889854731, 3935010590, 2197155094, 2130066389, 3598276897, 2308109491, 2382939200, 1228239371, 1884934581, 3466679822, 1211467195, 2977706375, 3144137970, 3080919767, 945692709, 3015121229, 345764865, 826997308, 2043625172, 2964804700, 2628071007, 4154339049, 483812778, 3288636719, 2696449880, 2122325384, 1371447954, 411563403, 3577634219, 976656727, 2708061387, 723783916, 3181552825, 3346694253, 3617629408, 1408862808, 519937465, 1323711759, 1474661346, 2773859924, 3580214553, 1143088323, 2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155, 2779020594,
};
const int32_t zetas_inv_asm[128] = {
  // 1 layer	
  1515946703, 3153169142, 508325959, 1572714069, 2967385035, 1877193577, 2731284400, 2073299023, 3151878974, 714752744, 1521107373, 2820305951, 2971255538, 3775029832, 2886104489, 677337889, 948273044, 1113414472, 3571183381, 1586905910, 3318310570, 717333078, 3883403894, 2923519343, 2172641913, 1598517417, 1006330578, 3811154519, 140628248, 1666896290, 1330162597, 2251342125, 3467969989, 3949202432, 1279846068, 3349274588, 1214047530, 1150829327, 1317260922, 3083500102, 828287475, 2410032716, 3066727926, 1912028097, 1986857806, 696690400, 2164900908, 2097812203, 359956707, 405112566, 1513366368, 3319600737, 3994358291, 61928036, 860541661, 2477121421, 2875783149, 1343064271, 3362176262, 2182963252, 877313837, 3542799699, 732815087, 4273034451, 
  // 1st loop of 2 & 3 & 4 layers
  3120914957, 2711931889, 282546663, 1103093133, 3235739856, 1748176836, 2042335005, 
  // 2nd loop of 2 & 3 & 4 layers
  3458938817, 2384229368, 918599194, 879894172, 580575333, 1207596693, 3560862042, 
  // 3rd loop of 2 & 3 & 4 layers
  1195985186, 2315850495, 594767175, 2439706566, 576704831, 3604727734, 89021552, 
  // 4th loop of 2 & 3 & 4 layers
  3711811629, 3253802200, 2443577068, 1303069081, 1593356747, 2455188575, 700560902, 
  // 5th loop of 2 & 3 & 4 layers
  1824296713, 2591946320, 4252391772, 598637677, 2575174144, 1404992306, 3266703874, 
  // 6th loop of 2 & 3 & 4 layers
  3479581496, 1052776604, 1744306334, 3456358482, 4222717922, 365117377, 4045964987, 
  // 7th loop of 2 & 3 & 4 layers
  1643673276, 1357256112, 1322421592, 2750636911, 2390680205, 2567433139, 1819136044, 
  // 8th loop of 2 & 3 & 4 layers
  2029433331, 98052723, 3867921885, 3922108916, 1137927653, 3913077744, 2924809511, 
  // 5 & 6 & 7 layers
  3200905336, 1847519727, 3482161830, 886345009, 51606697, 966335388, 1802363867, 
  // 128^-1 * (2^16)^2
  2435836064
};

extern void ntt_fast(int16_t *, const int32_t *);
/*************************************************
* Name:        ntt
*
* Description: Inplace number-theoretic transform (NTT) in Rq
*              input is in standard order, output is in bitreversed order
*
* Arguments:   - int16_t *poly: pointer to input/output vector of 256 elements of Zq
**************************************************/
void ntt(int16_t *poly) {
    ntt_fast(poly, zetas_asm);
}

extern void invntt_fast(int16_t *, const int32_t *);
/*************************************************
* Name:        invntt
*
* Description: Inplace inverse number-theoretic transform in Rq
*              input is in bitreversed order, output is in standard order
*
* Arguments:   - int16_t *poly: pointer to input/output vector of 256 elements of Zq
**************************************************/
void invntt(int16_t *poly) {
    invntt_fast(poly, zetas_inv_asm);
}
