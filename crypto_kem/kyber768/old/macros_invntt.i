/******************************************************************************
 * Integrating the improved Plantard arithmetic into Kyber.
 *
 * Efficient Plantard arithmetic enables a faster Kyber implementation with the
 * same stack usage.
 *
 * See the paper at https://eprint.iacr.org/2022/956.pdf for more details.
 *
 * @author   Junhao Huang, BNU-HKBU United International College, Zhuhai, China
 *           jhhuang_nuaa@126.com
 *
 * @date     September 2022
 ******************************************************************************/

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

.macro fqmulprecomp_plant a, twiddle, tmp, q, qa
	smulwb \tmp, \twiddle, \a 
	smulwt \a, \twiddle, \a
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a, \a, \q, \qa
	pkhtb \a, \a, \tmp, asr#16
.endm

#endif /* MACROS_INVNTT_I */
