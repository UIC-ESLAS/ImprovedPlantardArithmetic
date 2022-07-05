#include <stdint.h>
#include "params.h"
#include "fq.h"
#include "ntt.h"

static const unsigned int tree[256] = {1, 128, 64, 320, 32, 224, 160, 352, 16, 208, 112, 304, 80, 272, 176, 368, 8, 200, 104, 296, 56, 248, 152, 344, 40, 232, 136, 328, 88, 280, 184, 376, 4, 196, 100, 292, 52, 244, 148, 340, 28, 220, 124, 316, 76, 268, 172, 364, 20, 212, 116, 308, 68, 260, 164, 356, 44, 236, 140, 332, 92, 284, 188, 380, 2, 194, 98, 290, 50, 242, 146, 338, 26, 218, 122, 314, 74, 266, 170, 362, 14, 206, 110, 302, 62, 254, 158, 350, 38, 230, 134, 326, 86, 278, 182, 374, 10, 202, 106, 298, 58, 250, 154, 346, 34, 226, 130, 322, 82, 274, 178, 370, 22, 214, 118, 310, 70, 262, 166, 358, 46, 238, 142, 334, 94, 286, 190, 382, 1, 193, 97, 289, 49, 241, 145, 337, 25, 217, 121, 313, 73, 265, 169, 361, 13, 205, 109, 301, 61, 253, 157, 349, 37, 229, 133, 325, 85, 277, 181, 373, 7, 199, 103, 295, 55, 247, 151, 343, 31, 223, 127, 319, 79, 271, 175, 367, 19, 211, 115, 307, 67, 259, 163, 355, 43, 235, 139, 331, 91, 283, 187, 379, 5, 197, 101, 293, 53, 245, 149, 341, 29, 221, 125, 317, 77, 269, 173, 365, 17, 209, 113, 305, 65, 257, 161, 353, 41, 233, 137, 329, 89, 281, 185, 377, 11, 203, 107, 299, 59, 251, 155, 347, 35, 227, 131, 323, 83, 275, 179, 371, 23, 215, 119, 311, 71, 263, 167, 359, 47, 239, 143, 335, 95, 287, 191, 383};

// int16_t zetas[256];
// int16_t zetas_inv[256];
const int16_t zetas[256] = {-2731, -308, -1252, -2652, -1463, 2765, 2162, -1734, 221, -2586, -762, -2970, 2901, 2235, 1015, -338, 848, -3771, -630, 3653, -1398, -2062, 1727, 2800, 2327, 784, 3724, -1452, 2631, 1606, -3893, -2896, -3236, 1963, -277, 9, 190, 2434, 40, 2938, -3801, 789, -2013, -3068, -1614, -1029, 873, -3855, -3324, 108, 513, 427, 480, -3149, -1516, -2280, -1113, 1589, 1787, -474, 2795, -174, 3014, -3675, -853, -2357, 2246, -1709, 4033, -2183, -768, 1966, 2314, -1323, 3317, 530, 1801, -1750, 3209, -2794, -2707, 2029, 3877, 3257, -2077, -1624, -33, -3576, -3201, -1227, 3773, 1763, -2893, 1425, 1008, 300, 1236, -2924, 1473, 1810, 1513, -2933, -490, -1426, -1444, -64, -304, -822, 569, 3004, -1093, 3058, -787, -2886, -2187, 1818, -25, 84, 399, 2039, 298, -1923, 467, 2425, -3426, -471, -317, -2929, -2731, -1270, -2192, 3371, 2890, 1043, 3034, -2206, 2420, 1086, 1318, -3814, -294, 3753, -3296, -2444, 2553, -3355, 1346, 1315, -1256, 1455, -2690, -1715, 224, 2627, 2877, -1064, -2833, -1849, -3022, 15, -3030, -3645, -3872, 2871, -838, 665, -2602, 140, 1079, -1782, 3057, -3205, -239, 2032, 1971, -785, 1370, -3067, 2714, -2667, 24, 3299, -1612, -114, -2744, -3377, -2599, -2328, 2060, -2313, 2455, -2104, -3272, 855, 2141, 180, 3800, 2594, 800, -2688, 790, 418, -1855, 88, -1556, 2463, 2098, -290, -3261, 2047, 122, 2048, -1997, -3429, -2846, -3956, 454, 318, -2330, 1684, -1147, 1396, -1050, 3528, 1677, 2968, -1264, -2205, -464, 2788, -2119, 2204, 1844, -1280, 1601, -1078, 3699, -1368, 1183, -288, -378, -3953, 2346, -2045, -500, 1680, 299, 2375, -1721, -55, 1659, 2414, 609, -1739, 1341, 2868};

const int16_t zetas_inv[256] = {-2868, -1341, 1739, -609, -2414, -1659, 55, 1721, -2375, -299, -1680, 500, 2045, -2346, 3953, 378, 288, -1183, 1368, -3699, 1078, -1601, 1280, -1844, -2204, 2119, -2788, 464, 2205, 1264, -2968, -1677, -3528, 1050, -1396, 1147, -1684, 2330, -318, -454, 3956, 2846, 3429, 1997, -2048, -122, -2047, 3261, 290, -2098, -2463, 1556, -88, 1855, -418, -790, 2688, -800, -2594, -3800, -180, -2141, -855, 3272, 2104, -2455, 2313, -2060, 2328, 2599, 3377, 2744, 114, 1612, -3299, -24, 2667, -2714, 3067, -1370, 785, -1971, -2032, 239, 3205, -3057, 1782, -1079, -140, 2602, -665, 838, -2871, 3872, 3645, 3030, -15, 3022, 1849, 2833, 1064, -2877, -2627, -224, 1715, 2690, -1455, 1256, -1315, -1346, 3355, -2553, 2444, 3296, -3753, 294, 3814, -1318, -1086, -2420, 2206, -3034, -1043, -2890, -3371, 2192, 1270, 2731, 2929, 317, 471, 3426, -2425, -467, 1923, -298, -2039, -399, -84, 25, -1818, 2187, 2886, 787, -3058, 1093, -3004, -569, 822, 304, 64, 1444, 1426, 490, 2933, -1513, -1810, -1473, 2924, -1236, -300, -1008, -1425, 2893, -1763, -3773, 1227, 3201, 3576, 33, 1624, 2077, -3257, -3877, -2029, 2707, 2794, -3209, 1750, -1801, -530, -3317, 1323, -2314, -1966, 768, 2183, -4033, 1709, -2246, 2357, 853, 3675, -3014, 174, -2795, 474, -1787, -1589, 1113, 2280, 1516, 3149, -480, -427, -513, -108, 3324, 3855, -873, 1029, 1614, 3068, 2013, -789, 3801, -2938, -40, -2434, -190, -9, 277, -1963, 3236, 2896, 3893, -1606, -2631, 1452, -3724, -784, -2327, -2800, -1727, 2062, 1398, -3653, 630, 3771, -848, 338, -1015, -2235, -2901, 2970, 762, 2586, -221, 1734, -2162, -2765, 1463, 2652, 1252, 1568};
// void init_ntt() {
//   unsigned int i, j, k;
//   int16_t tmp[384];

//   tmp[0] = MONT;
//   for(i = 1; i < 384; ++i)
//     tmp[i] = fqmul(tmp[i-1], ROOT_OF_UNITY*MONT % Q);

//   for(i = 0; i < 256; ++i)
//     zetas[i] = tmp[tree[i]];

//   k = 0;
//   for(i = 128; i >= 2; i >>= 1)
//     for(j = i; j < 2*i; ++j)
//       zetas_inv[k++] = -tmp[384 - tree[j]];

//   zetas_inv[254] = 2*zetas[1] - tmp[0];
//   zetas_inv[254] = fqinv(zetas_inv[254]);
// }

void ntt(int16_t b[768], const int16_t a[768]) {
  unsigned int len, start, j, k;
  int16_t t, zeta;

  for(j = 0; j < 384; ++j) {
    t = fqmul(zetas[1], a[j + 384]);
    b[j + 384] = a[j] + a[j + 384] - t;
    b[j] = a[j] + t;
  }

  k = 2;
  for(len = 192; len >= 3; len >>= 1) {
    for(start = 0; start < 768; start = j + len) {
      zeta = zetas[k++];
      for(j = start; j < start + len; ++j) {
        t = fqmul(zeta, b[j + len]);
        b[j + len] = fqred16(b[j] - t);
        b[j] = fqred16(b[j] + t);
      }
    }
  }
}

void invntt(int16_t b[768], const int16_t a[768]) {
  unsigned int start, len, j, k;
  int16_t t, zeta;

  for(j = 0; j < 768; ++j)
    b[j] = a[j];

  k = 0;
  for(len = 3; len <= 192; len <<= 1) {
    for(start = 0; start < 768; start = j + len) {
      zeta = zetas_inv[k++];
      for(j = start; j < start + len; ++j) {
        t = b[j];
        b[j] = fqred16(t + b[j + len]);
        b[j + len] = t - b[j + len];
        b[j + len] = fqmul(zeta, b[j + len]);
      }
    }
  }

  for(j = 0; j < 384; ++j) {
    t = b[j] - b[j + 384];
    t = fqmul(zetas_inv[254], t);
    b[j] = b[j] + b[j + 384];
    b[j] = b[j] - t;
    b[j] = fqmul((1U << 24) % Q, b[j]);
    b[j + 384] = fqmul((1U << 25) % Q, t);
  }
}

void ntt_pack(int16_t b[768], const int16_t a[768]) {
  unsigned i, j, k;
  int16_t buf[96];

  for(i = 0; i < 768/96; ++i) {
    for(j = 0; j < 96; ++j)
      buf[j] = a[96*i + j];

    for(j = 0; j < 6; ++j)
      for(k = 0; k < 16; ++k)
        b[96*i + 16*j + k] = buf[6*k + j];
  }
}

void ntt_unpack(int16_t b[768], const int16_t a[768]) {
  unsigned j, k, l;
  int16_t buf[96];

  for(j = 0; j < 768/96; ++j) {
    for(k = 0; k < 6; ++k)
      for(l = 0; l < 16; ++l)
        buf[6*l + k] = a[96*j + 16*k + l];

    for(k = 0; k < 96; ++k)
      b[96*j + k] = buf[k];
  }
}

void basemul(int16_t c[3],
             const int16_t a[3],
             const int16_t b[3],
             int16_t zeta)
{
  c[0]  = fqmul(a[2], b[1]);
  c[0] += fqmul(a[1], b[2]);
  c[0]  = fqmul(c[0], zeta);
  c[0] += fqmul(a[0], b[0]);

  c[1]  = fqmul(a[2], b[2]);
  c[1]  = fqmul(c[1], zeta);
  c[1] += fqmul(a[0], b[1]);
  c[1] += fqmul(a[1], b[0]);
  //c[1]  = fqred16(c[1]);

  c[2]  = fqmul(a[2], b[0]);
  c[2] += fqmul(a[1], b[1]);
  c[2] += fqmul(a[0], b[2]);
  //c[2]  = fqred16(c[2]);
}

int baseinv(int16_t b[3], const int16_t a[3], int16_t zeta) {
  int16_t det, t;
  int r;

  b[0]  = fqmul(a[0], a[0]);
  t     = fqmul(a[1], a[2]);
  t     = fqmul(t, zeta);
  b[0] -= t;

  b[1]  = fqmul(a[2], a[2]);
  b[1]  = fqmul(b[1], zeta);
  t     = fqmul(a[0], a[1]);
  b[1] -= t;

  b[2]  = fqmul(a[1], a[1]);
  t     = fqmul(a[0], a[2]);
  b[2] -= t;

  det   = fqmul(b[2], a[1]);
  t     = fqmul(b[1], a[2]);
  det  += t;
  det   = fqmul(det, zeta);
  t     = fqmul(b[0], a[0]);
  det  += t;

  det   = fqinv(det);
  b[0]  = fqmul(b[0], det);
  b[1]  = fqmul(b[1], det);
  b[2]  = fqmul(b[2], det);

  r = (uint16_t)det;
  r = (uint32_t)(-r) >> 31;
  return r - 1;
}
