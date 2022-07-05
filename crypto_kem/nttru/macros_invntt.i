/*
 * @Author: Junhao Huang
 * @Date: 2021-09-09 19:09:30
 * @LastEditTime: 2021-10-08 20:41:51
 * @LastEditors: Please set LastEditors
 * @Description: Integrate Plantard Modular reduction
 * @FilePath: Kyber_M4\kyber\macros_invntt.i
 */

#ifndef MACROS_INVNTT_I
#define MACROS_INVNTT_I

#include "macros.i"

.macro doubleinvbutterfly_plant a0, a1, twiddle, tmp, q, qa
	usub16 \tmp, \a0, \a1
	uadd16 \a0, \a0, \a1

	smulwb \a1, \twiddle, \tmp
	smulwt \tmp, \twiddle, \tmp
	smlabt \a1, \a1, \q, \qa
	smlabt \tmp, \tmp, \q, \qa
	pkhtb \a1, \tmp, \a1, asr#16
.endm

.macro two_doubleinvbutterfly_plant a0, a1, a2, a3, twiddle1, twiddle2, tmp, q, qa
	doubleinvbutterfly_plant \a0, \a1, \twiddle1, \tmp, \q, \qa
	doubleinvbutterfly_plant \a2, \a3, \twiddle2, \tmp, \q, \qa
.endm

// a0, a1 input range: (-2q,2q)
.macro doubleinvbutterfly_plant_last_layer a0, a1, twiddle1, twiddle2, twiddle3, tmp, q, qa
	usub16 \tmp, \a0, \a1 // a0-a1
	uadd16 \a0, \a0, \a1 // a0+a1

	smulwb \a1, \twiddle1, \tmp
	smulwt \tmp, \twiddle1, \tmp
	smlabt \a1, \a1, \q, \qa
	smlabt \tmp, \tmp, \q, \qa 
	pkhtb \a1, \tmp, \a1, asr#16 //(a0-a1)*zeta
	usub16 \a0, \a0, \a1 //+0.5q?
	// b[j] = fqmul(PLANT^2 * 256^-1 % Q, b[j]);
	smulwb \tmp, \twiddle2, \a0
	smulwt \a0, \twiddle2, \a0
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a0, \a0, \q, \qa
	pkhtb \a0, \a0, \tmp, asr#16
	// b[j + 384] = fqmul(PLANT^2 * 256^-1 * 2 % Q, t);
	smulwb \tmp, \twiddle3, \a1 
	smulwt \a1, \twiddle3, \a1 
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a1, \a1, \q, \qa
	pkhtb \a1, \a1, \tmp, asr#16
.endm

.macro two_doubleinvbutterfly_plant_last_layer a0, a1, a2, a3, twiddle1, twiddle2, twiddle3, tmp, q, qa
	doubleinvbutterfly_plant_last_layer \a0, \a1, \twiddle1, \twiddle2, \twiddle3, \tmp, \q, \qa
	doubleinvbutterfly_plant_last_layer \a2, \a3, \twiddle1, \twiddle2, \twiddle3, \tmp, \q, \qa
.endm

.macro singleinvbutterfly_plant a0, a1, twiddle, tmp, q, qa
	usub16 \tmp, \a0, \a1
	uadd16 \a0, \a0, \a1
	smulwb \a1, \twiddle, \tmp
	smlabt \a1, \a1, \q, \qa
	asr \a1, \a1, #16
.endm

.macro two_singleinvbutterfly_plant a0, a1, a2, a3, twiddle1, twiddle2, tmp, q, qa
	singleinvbutterfly_plant \a0, \a1, \twiddle1, \tmp, \q, \qa
	singleinvbutterfly_plant \a2, \a3, \twiddle2, \tmp, \q, \qa
.endm

.macro fqmulprecomp_plant a, twiddle, tmp, q, qa
	smulwb \tmp, \twiddle, \a 
	smulwt \a, \twiddle, \a
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a, \a, \q, \qa
	pkhtb \a, \a, \tmp, asr#16
.endm

#endif /* MACROS_INVNTT_I */
