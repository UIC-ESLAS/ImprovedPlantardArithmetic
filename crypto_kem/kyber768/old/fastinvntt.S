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

#include "macros.i"
#include "macros_invntt.i"

.syntax unified
.cpu cortex-m4
.thumb

.macro fullplant a0, a1, a2, a3, tmp, q, qa, plantconst
	movw \plantconst, #44984
	movt \plantconst, #19
	doubleplant \a0, \tmp, \q, \qa, \plantconst
	doubleplant \a1, \tmp, \q, \qa, \plantconst
	doubleplant \a2, \tmp, \q, \qa, \plantconst
	doubleplant \a3, \tmp, \q, \qa, \plantconst
.endm
.macro halfplant a0, a1, tmp, q, qa, plantconst
	movw \plantconst, #44984
	movt \plantconst, #19
	doubleplant \a0, \tmp, \q, \qa, \plantconst
	doubleplant \a1, \tmp, \q, \qa, \plantconst
.endm

.global invntt_fast
.type invntt_fast, %function
.align 2
invntt_fast:
	push {r4-r11, lr}

	poly        .req r0
	twiddle_ptr .req r1
	poly0       .req r2
	poly1       .req r3
	poly2       .req r4
	poly3       .req r5
	poly4       .req r6
	poly5       .req r7
	poly6       .req r8
	poly7       .req r9
	twiddle1    .req r10
	twiddle2    .req r11
	plantconst  .req r10
	###  qinv        .req r11 ### q^-1 mod 2^2n; n=16
	q           .req r12 
	### at the top of r12
	qa          .req r0
	### qa=2^a q;a=3; at the bottom of r0
	tmp         .req r14

	// movw qa, #26632
	movt q, #3329

	### LAYER 1 (skip layer 0)
#ifdef USE_REPT
	.rept 16
#else
	movw tmp, #16
	1:
		push {tmp}
#endif

	vmov.w s0, poly
	ldm poly, {poly0-poly7}
	movw qa, #26632

	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr], #8 
	two_doubleinvbutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr], #8 
	two_doubleinvbutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	vmov.w poly, s0

	stm poly!, {poly0-poly7}

#ifdef USE_REPT
	.endr
#else
	pop {tmp}
	subs.w tmp, #1
	bne.w 1b
#endif

	sub.w poly, #512

	.equ distance, 32

	### LAYER 2+3+4
#ifdef USE_REPT
	.rept 8
#else
	movw tmp, #8
	2:
		push {tmp}
#endif
	vmov.w s0, poly
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	movw qa, #26632

	ldrd twiddle1, twiddle2, [twiddle_ptr] 
	two_doubleinvbutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr, #8] 
	two_doubleinvbutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	fullplant poly0, poly2, poly4, poly6, tmp, q, qa, plantconst

	ldrd twiddle1, twiddle2, [twiddle_ptr, #16] 
	two_doubleinvbutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa

	ldr.w twiddle1, [twiddle_ptr, #24] 
	two_doubleinvbutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa

	vmov.w poly, s0

	store poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	str.w poly1, [poly, #distance/4]
	str.w poly2, [poly, #2*distance/4]
	str.w poly3, [poly, #3*distance/4]
	str.w poly0, [poly], #4


	vmov.w s0, poly
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	movw qa, #26632

	ldrd twiddle1, twiddle2, [twiddle_ptr], #8
	two_doubleinvbutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr], #8 
	two_doubleinvbutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	fullplant poly0, poly2, poly4, poly6, tmp, q, qa, plantconst

	ldrd twiddle1, twiddle2, [twiddle_ptr], #8
	two_doubleinvbutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa

	// ldr.w twiddle, [twiddle_ptr], #4
	ldr.w twiddle1, [twiddle_ptr], #4 
	two_doubleinvbutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa

	vmov.w poly,s0
// original
	store poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	str.w poly1, [poly, #distance/4]
	str.w poly2, [poly, #2*distance/4]
	str.w poly3, [poly, #3*distance/4]
	str.w poly0, [poly], #60

#ifdef USE_REPT
	.endr
#else
		pop {tmp}
		subs.w tmp, #1
	bne.w 2b
#endif

	sub.w poly, #512

	.equ distance, 8*distance

	### LAYER 5+6+7: 4 times
#ifdef USE_REPT
	.rept 4
#else
	movw tmp, #4
	3:
		push {tmp}
#endif
	vmov.w s0, poly
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	movw qa, #26632

	ldrd twiddle1, twiddle2, [twiddle_ptr] 
	two_doubleinvbutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr, #8] 
	two_doubleinvbutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #16] 
	two_doubleinvbutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	// only need to reduce 4 times
	halfplant poly0, poly4, tmp, q, qa, plantconst
	// fullplant poly0, poly1, poly4, poly5, tmp, q, qa, plantconst

	ldrd twiddle1, twiddle2, [twiddle_ptr, #24] 
	two_doubleinvbutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa

	fqmulprecomp_plant poly0, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly1, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly2, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly3, twiddle2, tmp, q, qa

	vmov.w poly, s0

	store poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	str.w poly1, [poly, #distance/4]
	str.w poly2, [poly, #2*distance/4]
	str.w poly3, [poly, #3*distance/4]
	str.w poly0, [poly], #4

#ifdef USE_REPT
	.endr
#else
		pop {tmp}
		subs.w tmp, #1
	bne.w 3b
#endif

#ifdef USE_REPT
	.rept 12
#else
	movw tmp, #12
	4:
		push {tmp}
#endif
	vmov.w s0, poly
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	movw qa, #26632

	ldrd twiddle1, twiddle2, [twiddle_ptr] 
	two_doubleinvbutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4
	ldrd twiddle1, twiddle2, [twiddle_ptr, #8] 
	two_doubleinvbutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #16] 
	two_doubleinvbutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa
	// ldr.w twiddle, [twiddle_ptr], #4

	// halfplant poly0, poly4, tmp, q, qa, plantconst
	// fullplant poly0, poly1, poly4, poly5, tmp, q, qa, plantconst

	ldrd twiddle1, twiddle2, [twiddle_ptr, #24] 
	two_doubleinvbutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doubleinvbutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa

	// @TODO
	fqmulprecomp_plant poly0, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly1, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly2, twiddle2, tmp, q, qa
	fqmulprecomp_plant poly3, twiddle2, tmp, q, qa

	vmov.w poly, s0

	store poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	str.w poly1, [poly, #distance/4]
	str.w poly2, [poly, #2*distance/4]
	str.w poly3, [poly, #3*distance/4]
	str.w poly0, [poly], #4

#ifdef USE_REPT
	.endr
#else
	pop {tmp}
	subs.w tmp, #1
	bne.w 4b
#endif

	pop {r4-r11, pc}
