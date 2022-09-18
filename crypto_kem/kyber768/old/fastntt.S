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
#include "macros_ntt.i"

.syntax unified
.cpu cortex-m4
.thumb

.global ntt_fast
.type ntt_fast, %function
.align 2
ntt_fast:
	push {r4-r11, r14}

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
	###  qinv        .req r11 ### q^-1 mod 2^2n; n=16
	q           .req r12 
	### at the top of r12
	qa          .req r0
	### qa=2^a q;a=3; at the bottom of r12
	tmp         .req r14

	// movw qa, #26632
	// Why movt? Because we initially placed qa at the bottom of the same register as q;
	movt q, #3329
	
	.equ distance, 256

	### LAYER 7+6+5
#ifdef USE_REPT
	.rept 16
#else
	movw tmp, #16
	1:
		push {tmp}
#endif

	vmov.w s0, poly
	### increase 3cc for ldrd while reduce 24cc for each loop
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
	
	// push {poly}
	
	movw qa, #26632

	ldr.w twiddle1, [twiddle_ptr] 
	### do not increase cc while reduce 8cc
	two_doublebutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doublebutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #4] 
	### increase 1cc while reduce 8cc
	two_doublebutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doublebutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #12] 
	### increase 1cc while reduce 4cc
	two_doublebutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #20] 
	### increase 1cc while reduce 4cc
	two_doublebutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	// pop {poly}
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
	bne.w 1b
#endif

	sub.w poly, #64
	add.w twiddle_ptr, #28

	.equ distance, distance/8

	### LAYER 4+3+2
#ifdef USE_REPT
	.rept 8
#else
	movw tmp, #8
	2:
		push {tmp}
#endif

#ifdef USE_REPT
	.rept 2
#else
	movw tmp, #2
	3:
		push {tmp}
#endif
	vmov.w s0, poly
	### increase 3cc for ldrd while reduce 24cc for each loop
	load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
	load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4

	// push {poly}
	
	movw qa, #26632
	
	ldr twiddle1, [twiddle_ptr] 
	### do not increase cc while reduce 8cc
	two_doublebutterfly_plant poly0, poly4, poly1, poly5, twiddle1, twiddle1, tmp, q, qa
	two_doublebutterfly_plant poly2, poly6, poly3, poly7, twiddle1, twiddle1, tmp, q, qa
	
	ldrd twiddle1, twiddle2, [twiddle_ptr, #4] 
	### increase 1cc while reduce 8cc
	two_doublebutterfly_plant poly0, poly2, poly1, poly3, twiddle1, twiddle1, tmp, q, qa
	two_doublebutterfly_plant poly4, poly6, poly5, poly7, twiddle2, twiddle2, tmp, q, qa


	ldrd twiddle1, twiddle2, [twiddle_ptr, #12] 
	### increase 1cc while reduce 4cc
	two_doublebutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr, #20] 
	### increase 1cc while reduce 4cc
	two_doublebutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa

	// pop {poly}
	vmov.w poly, s0
	store poly, poly4, poly5, poly6, poly7, #distance,#5*distance/4, #6*distance/4, #7*distance/4
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

	add.w poly, #56
	add.w twiddle_ptr, #28

#ifdef USE_REPT
	.endr
#else
	pop {tmp}
	subs.w tmp, #1
	bne.w 2b
#endif

	sub poly, #512

	### LAYER 1 (skip layer 0)
#ifdef USE_REPT
	.rept 16
#else
	movw tmp, #16
	4:
		push {tmp}
#endif

	vmov.w s0, poly
	ldm poly, {poly0-poly7}

	// push {poly}
	
	movw qa, #26632

	ldrd twiddle1, twiddle2, [twiddle_ptr], #8
	### ldr.w twiddle, [twiddle_ptr], #4
	two_doublebutterfly_plant poly0, poly1, poly2, poly3, twiddle1, twiddle2, tmp, q, qa

	ldrd twiddle1, twiddle2, [twiddle_ptr], #8
	### ldr.w twiddle, [twiddle_ptr], #4
	two_doublebutterfly_plant poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, q, qa
	
	// pop {poly}
	vmov.w poly, s0
	stm poly!, {poly0-poly7}

#ifdef USE_REPT
	.endr
#else
	pop {tmp}
	subs.w tmp, #1
	bne.w 4b
#endif

	pop {r4-r11, pc}
