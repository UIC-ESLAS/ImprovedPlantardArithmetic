/*
 * @Author: Junhao Huang
 * @Date: 2021-09-09 19:09:30
 * @LastEditTime: 2022-02-14 06:53:29
 * @LastEditors: Please set LastEditors
 * @Description: Integrate Plantard Modular reduction
 * @FilePath: \Kyber_M4\kyber\macros_ntt.i
 */

#ifndef MACROS_NTT_I
#define MACROS_NTT_I

#include "macros.i"
#include "params.h"

### 7 instructions
.macro doublebutterfly_plant a0, a1, twiddle, tmp, q, qa
	smulwb \tmp, \twiddle, \a1 
	smulwt \a1, \twiddle, \a1
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a1, \a1, \q, \qa
	pkhtb \tmp, \a1, \tmp, asr#16
	usub16 \a1, \a0, \tmp
	uadd16 \a0, \a0, \tmp
.endm

.macro two_doublebutterfly_plant a0, a1, a2, a3, twiddle0, twiddle1, tmp, q, qa
	doublebutterfly_plant \a0, \a1, \twiddle0, \tmp, \q, \qa
	doublebutterfly_plant \a2, \a3, \twiddle1, \tmp, \q, \qa
.endm

#endif /* MACROS_NTT_I */
