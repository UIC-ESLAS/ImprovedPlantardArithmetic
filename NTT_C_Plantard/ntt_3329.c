#include <stdint.h>
#include "stdio.h"

#define CONST_PLANT 1976  //-2^32 mod q
#define Q 3329
#define Qinv_Plant 1806234369

// Multiplication by a constant, where b is pre-multiplied by Qinv_Plant.
int16_t plant_mul(int32_t a, int32_t b){
  int32_t t;
  t=(int32_t)a*b;
  t>>=16;
  t=(t+8)*Q;
  t>>=16;
  return t;
}

int16_t plant_red(int32_t a){
  int32_t t;
  t=(int32_t)a*Qinv_Plant;
  t>>=16;
  t=(t+8)*Q;
  t>>=16;
  return t;
}

const uint32_t zetas_plantard_cpu[128] = {
    1290168, 2230699446, 3328631909, 4243360600, 3408622288, 812805467, 2447447570, 1094061961, 1370157786, 2475831253, 249002310,
    1028263423, 3594406395, 4205945745, 734105255, 2252632292, 381889553, 3157039644, 1727534158, 1904287092, 3929849920, 72249375,
    2889974991, 1719793153, 1839778722, 2701610550, 690239563, 3718262466, 3087370604, 3714391964, 2546790461, 1059227441,
    372858381, 427045412, 4196914574, 2265533966, 1544330386, 2972545705, 2937711185, 2651294021, 838608815, 2550660963, 3242190693,
    815385801, 3696329620, 42575525, 1703020977, 2470670584, 2991898216, 1851390229, 1041165097, 583155668, 1855260731, 3700200122,
    1979116802, 3098982111, 3415073125, 3376368103, 1910737929, 836028480, 3191874164, 4012420634, 1583035408, 1174052340,
    21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026, 1419184148, 1817845876, 3434425636,
    4233039261, 300609006, 975366560, 2781600929, 3889854731, 3935010590, 2197155094, 2130066389, 3598276897, 2308109491, 2382939200,
    1228239371, 1884934581, 3466679822, 1211467195, 2977706375, 3144137970, 3080919767, 945692709, 3015121229, 345764865, 826997308,
    2043625172, 2964804700, 2628071007, 4154339049, 483812778, 3288636719, 2696449880, 2122325384, 1371447954, 411563403, 3577634219,
    976656727, 2708061387, 723783916, 3181552825, 3346694253, 3617629408, 1408862808, 519937465, 1323711759, 1474661346, 2773859924,
    3580214553, 1143088323, 2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155, 2779020594};

void ntt(int16_t r[256]) {
  unsigned int len, start, j, k;
  int16_t t;
  uint32_t zeta;

  k = 1;
 
  for(len = 128; len>=2; len >>= 1) {     
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas_plantard_cpu[k++];
      for(j = start; j < start + len; ++j) {
        t = plant_mul(r[j + len],zeta); 
        r[j + len] = r[j] - t;
        r[j] = r[j] + t;
      }
    }
  }
}

void basemul(int16_t r[2],int16_t a[2],int16_t b[2],uint32_t zeta){
  r[0]  = plant_red(a[1]*b[1]);
  r[0]  = plant_mul(r[0],zeta);
  r[0] += plant_red(a[0]*b[0]);

  r[1]  = plant_red(a[0]*b[1]);
  r[1] += plant_red(a[1]*b[0]);
}

void pointwise_multiplication(int16_t *c, int16_t *a, int16_t *b){
  for(int i=0;i<256/4;i++){
    basemul(c+4*i, a+4*i, b+4*i, zetas_plantard_cpu[64+i]);
    basemul(c+4*i+2, a+4*i+2, b+4*i+2 ,-zetas_plantard_cpu[64+i]);
  }
}

void invntt(int16_t r[256]) {
  unsigned int start, len, j, k;
  int16_t t;
  uint32_t zeta;
  uint32_t f = 2435836064; //QINV_PLANT*PLANT^2/128
  k = 127;
  for(len = 2; len <= 128; len <<= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas_plantard_cpu[k--];
      for(j = start; j < start + len; ++j) {
        t = r[j];
        r[j] = plant_red((int32_t)((t + r[j + len]) * CONST_PLANT));
        r[j + len] = r[j + len] - t;
        r[j + len] = plant_mul(r[j + len],zeta);
      }
    }
  }
  for(j = 0; j < 256; ++j){
    r[j] = plant_mul(r[j],f);
  }
}

void print_array(int16_t *a){
  for(int i=0;i<256;i++){
    printf("%d, ", (int32_t)a[i]);
  }
  printf("\n");
}

int main(){
  int16_t a[256]={0},b[256]={0},c[256]={0};
  b[0]=1;
  for(int i=0;i<256;i++){
    a[i]=i;
  }
  print_array(a);
  print_array(b);
  ntt(a);
  ntt(b);
  print_array(a);
  print_array(b);
  pointwise_multiplication(c,a,b);
  print_array(c);
  invntt(c);
  print_array(c);
  return 0;
}