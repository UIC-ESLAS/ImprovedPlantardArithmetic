/******************************************************************************
* Integrating the improved Plantard arithmetic into NTTRU.
*
* Efficient Plantard arithmetic enables a faster NTTRU implementation with the 
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

.macro mul_twiddle_plant a, twiddle, tmp, q, qa
	smulwb \tmp, \twiddle, \a
	smulwt \a,   \twiddle, \a
	smlabt \tmp, \tmp, \q, \qa
	smlabt \a, \a, \q, \qa
	pkhtb \a, \a, \tmp, asr#16
.endm

// a_i=a_i||a_(i+6); a_i in lower half 
.macro pkhbt_coeffs a0, a1, a2, a3, a4, a5, a6, a7
	pkhbt \a0, \a0, \a4, lsl#16
	pkhbt \a1, \a1, \a5, lsl#16
	pkhbt \a2, \a2, \a6, lsl#16
	pkhbt \a3, \a3, \a7, lsl#16
.endm

// b=a_bottom-a_top*twiddle; a=a_bottom+a_top*twiddle 
.macro CS_butterfly_unpack a, b, twiddle, tmp, q, qa
		smulwt \tmp, \twiddle, \a
		smlabt \tmp, \tmp, \q, \qa
		lsr \tmp, \tmp, #16
		usub16 \b, \a, \tmp
		uadd16 \a, \a, \tmp
.endm

.macro fullplant a0, a1, a2, a3, a4, a5, a6, a7, tmp, q, qa, plantconst
	movw \plantconst, #34880
	movt \plantconst, #8
	doubleplant \a0, \tmp, \q, \qa, \plantconst
	doubleplant \a1, \tmp, \q, \qa, \plantconst
	doubleplant \a2, \tmp, \q, \qa, \plantconst
	doubleplant \a3, \tmp, \q, \qa, \plantconst
	doubleplant \a4, \tmp, \q, \qa, \plantconst
	doubleplant \a5, \tmp, \q, \qa, \plantconst
	doubleplant \a6, \tmp, \q, \qa, \plantconst
	doubleplant \a7, \tmp, \q, \qa, \plantconst
.endm

.macro plant_low a, tmp, q, qa, plantconst
	smulwb \tmp, \plantconst, \a
	smlabt \tmp, \tmp, \q, \qa
	pkhtb \a, \a, \tmp, asr#16
.endm

.macro fullplant_low a0, a1, a2, a3, a4, a5, a6, a7, tmp, q, qa, plantconst
	movw \plantconst, #34880
	movt \plantconst, #8
	plant_low \a0, \tmp, \q, \qa, \plantconst
	plant_low \a1, \tmp, \q, \qa, \plantconst
	plant_low \a2, \tmp, \q, \qa, \plantconst
	plant_low \a3, \tmp, \q, \qa, \plantconst
	plant_low \a4, \tmp, \q, \qa, \plantconst
	plant_low \a5, \tmp, \q, \qa, \plantconst
	plant_low \a6, \tmp, \q, \qa, \plantconst
	plant_low \a7, \tmp, \q, \qa, \plantconst
.endm

.macro _3_layer_double_CT_16_plant_fp_first_layer c0, c1, c2, c3, c4, c5, c6, c7, xi0, xi1, xi2, xi3, xi4, xi5, xi6, twiddle1, twiddle2, q, qa, tmp
		// layer 3
		vmov \twiddle1, \xi0
		// twiddle2 here acts as a tmp register
		two_doublebutterfly_plant_first_layer \c0, \c4, \c1, \c5, \twiddle1, \tmp, \twiddle2, \q, \qa
		two_doublebutterfly_plant_first_layer \c2, \c6, \c3, \c7, \twiddle1, \tmp, \twiddle2, \q, \qa

		// layer 2
		vmov \twiddle1, \xi1
		vmov \twiddle2, \xi2
		two_doublebutterfly_plant \c0, \c2, \c1, \c3, \twiddle1, \twiddle1, \tmp, \q, \qa

		two_doublebutterfly_plant \c4, \c6, \c5, \c7, \twiddle2, \twiddle2, \tmp, \q, \qa

		// layer 1
		vmov \twiddle1, \xi3
		vmov \twiddle2, \xi4
		two_doublebutterfly_plant \c0, \c1, \c2, \c3, \twiddle1, \twiddle2, \tmp, \q, \qa

		vmov \twiddle1, \xi5
		vmov \twiddle2, \xi6
		two_doublebutterfly_plant \c4, \c5, \c6, \c7, \twiddle1, \twiddle2, \tmp, \q, \qa
.endm

.macro _3_layer_double_CT_16_plant_fp c0, c1, c2, c3, c4, c5, c6, c7, xi0, xi1, xi2, xi3, xi4, xi5, xi6, twiddle1, twiddle2, q, qa, tmp
		// layer 3
		vmov \twiddle1, \xi0
		two_doublebutterfly_plant \c0, \c4, \c1, \c5, \twiddle1, \twiddle1, \tmp, \q, \qa
		two_doublebutterfly_plant \c2, \c6, \c3, \c7, \twiddle1, \twiddle1, \tmp, \q, \qa

		// layer 2
		vmov \twiddle1, \xi1
		vmov \twiddle2, \xi2
		two_doublebutterfly_plant \c0, \c2, \c1, \c3, \twiddle1, \twiddle1, \tmp, \q, \qa

		two_doublebutterfly_plant \c4, \c6, \c5, \c7, \twiddle2, \twiddle2, \tmp, \q, \qa

		// layer 1
		vmov \twiddle1, \xi3
		vmov \twiddle2, \xi4
		two_doublebutterfly_plant \c0, \c1, \c2, \c3, \twiddle1, \twiddle2, \tmp, \q, \qa

		vmov \twiddle1, \xi5
		vmov \twiddle2, \xi6
		two_doublebutterfly_plant \c4, \c5, \c6, \c7, \twiddle1, \twiddle2, \tmp, \q, \qa
.endm

.global ntt_fast
.type ntt_fast, %function
.align 2
ntt_fast:
	push {r4-r11, r14}
	vpush.w {s16-s24}
	poly         .req r0
	twiddle_ptr  .req r1
	poly0        .req r2
	poly1        .req r3
	poly2        .req r4
	poly3        .req r5
	poly4        .req r6
	poly5        .req r7
	poly6        .req r8
	poly7        .req r9
	twiddle1     .req r10
	twiddle2     .req r11
	plantconst  .req r10
	###  qinv        .req r11 ### q^-1 mod 2^2n; n=16
	q           .req r12 
	### at the top of r12
	qa          .req r1
	### qa=2^a q;a=2
	tmp         .req r14

	// movw qa, #30724
	// Why movt? Because we initially placed qa at the bottom of the same register as q;
	movt q, #7681
	
	### LAYER 7+6+5+4
	.equ distance, 768
	.equ offset, 96
	.equ strincr, 4 //4*24=96
	// pre-load 15 twiddle factors to 15 FPU registers
	// s0-s7 used to temporary store 16 16-bit polys.
	vldm twiddle_ptr!, {s8-s22}
	vmov s23, twiddle_ptr
	movw qa, #30724 
	add tmp, poly, #strincr*24
	// s23: twiddle addr
	// s24: tmp  
	vmov s24, tmp  
	1:
		// load a1, a3, ..., a15
		load poly, poly0, poly1, poly2, poly3, #offset, #distance/4+offset, #2*distance/4+offset, #3*distance/4+offset
		load poly, poly4, poly5, poly6, poly7, #distance+offset, #5*distance/4+offset, #6*distance/4+offset, #7*distance/4+offset

		// 8-NTT on a1, a3, ..., a15
		_3_layer_double_CT_16_plant_fp_first_layer poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, s8, s9, s10, s11, s12, s13, s14, twiddle1, twiddle2, q, qa, tmp

		// s15-s22 left
		// multiply coeffs by layer 8 twiddles for later use
		vmov twiddle1, s15 
		vmov twiddle2, s16 
		mul_twiddle_plant poly0, twiddle1, tmp, q, qa
		mul_twiddle_plant poly1, twiddle2, tmp, q, qa

		vmov twiddle1, s17 
		vmov twiddle2, s18 
		mul_twiddle_plant poly2, twiddle1, tmp, q, qa
		mul_twiddle_plant poly3, twiddle2, tmp, q, qa

		vmov twiddle1, s19 
		vmov twiddle2, s20 
		mul_twiddle_plant poly4, twiddle1, tmp, q, qa
		mul_twiddle_plant poly5, twiddle2, tmp, q, qa

		vmov twiddle1, s21 
		vmov twiddle2, s22 
		mul_twiddle_plant poly6, twiddle1, tmp, q, qa
		mul_twiddle_plant poly7, twiddle2, tmp, q, qa

		vmov s0, poly0 // a1
		vmov s1, poly1 // a3
		vmov s2, poly2 // a5
		vmov s3, poly3 // a7
		vmov s4, poly4 // a9
		vmov s5, poly5 // a11
		vmov s6, poly6 // a13
		vmov s7, poly7 // a15
	
		// load a0, a2, ..., a14
		load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
		load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
		
		// 8-NTT on a0, a2, ..., a14
		_3_layer_double_CT_16_plant_fp_first_layer poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, s8, s9, s10, s11, s12, s13, s14, twiddle1, twiddle2, q, qa, tmp
		// fullplant poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, tmp, q, qa, plantconst
		
		// layer 4 - 1
		// addsub: (a2, a6, a10, a14), (a3, a7, a11, a15)
		vmov twiddle1, s1 // load a3
		uadd16 tmp, poly1, twiddle1
		usub16 poly1, poly1, twiddle1
		str.w tmp, [poly, #1*distance/4]
		str.w poly1, [poly, #1*distance/4+offset]

		vmov twiddle1, s3 // load a7
		uadd16 tmp, poly3, twiddle1
		usub16 poly3, poly3, twiddle1
		str.w tmp, [poly, #3*distance/4]
		str.w poly3, [poly, #3*distance/4+offset]
		
		vmov twiddle1, s5 // load a11
		uadd16 tmp, poly5, twiddle1
		usub16 poly5, poly5, twiddle1
		str.w tmp, [poly, #5*distance/4]
		str.w poly5, [poly, #5*distance/4+offset]
		
		vmov twiddle1, s7 // load a15
		uadd16 tmp, poly7, twiddle1
		usub16 poly7, poly7, twiddle1
		str.w tmp, [poly, #7*distance/4]
		str.w poly7, [poly, #7*distance/4+offset]
		
		// layer 4 - 2    
		// addsub: (a0, a4, a8, a12), (a1, a5, a9, a13)
		vmov poly3, s2 // load a5
		uadd16 tmp, poly2, poly3
		usub16 twiddle1, poly2, poly3
		str.w tmp, [poly, #2*distance/4]
		str.w twiddle1, [poly, #2*distance/4+offset]

		vmov poly5, s4 // load a9
		uadd16 tmp, poly4, poly5
		usub16 twiddle1, poly4, poly5
		str.w tmp, [poly, #4*distance/4]
		str.w twiddle1, [poly, #4*distance/4+offset]

		vmov poly7, s6 // load a13
		uadd16 tmp, poly6, poly7
		usub16 twiddle1, poly6, poly7
		str.w tmp, [poly, #6*distance/4]
		str.w twiddle1, [poly, #6*distance/4+offset]
		
		vmov poly1, s0 // load a1
		uadd16 tmp, poly0, poly1
		usub16 twiddle1, poly0, poly1
		str.w twiddle1, [poly, #offset]
		str.w tmp, [poly], #4

		vmov tmp, s24
		cmp.w poly, tmp
	bne.w 1b

	sub.w poly, #24*strincr

	### LAYER 3+2+1+0

	.equ distance, distance/16  //48
	.equ offset, 6
	.equ strincr, 96

	add.w tmp, poly, #strincr*16
	vmov s24, tmp
	2:
		vmov twiddle_ptr, s23
		vldm twiddle_ptr!, {s8-s22}
		vmov s23, twiddle_ptr
		movw qa, #30724 
		// movw tmp, #3
		// 3:
		// vmov s25, tmp
		load poly, poly0, poly1, poly2, poly3, #offset, #distance/4+offset, #2*distance/4+offset, #3*distance/4+offset
		load poly, poly4, poly5, poly6, poly7, #distance+offset, #5*distance/4+offset, #6*distance/4+offset, #7*distance/4+offset

		// 8-NTT on a1, a3, ..., a15
		_3_layer_double_CT_16_plant_fp poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, s8, s9, s10, s11, s12, s13, s14, twiddle1, twiddle2, q, qa, tmp

		// s15, s16, s17, s18, s19, s20, s21, s22 left
		// multiply coeffs by layer 0 twiddles for later use
		vmov twiddle1, s15 
		vmov twiddle2, s16 
		mul_twiddle_plant poly0, twiddle1, tmp, q, qa
		mul_twiddle_plant poly1, twiddle2, tmp, q, qa

		vmov twiddle1, s17 
		vmov twiddle2, s18 
		mul_twiddle_plant poly2, twiddle1, tmp, q, qa
		mul_twiddle_plant poly3, twiddle2, tmp, q, qa

		vmov twiddle1, s19 
		vmov twiddle2, s20 
		mul_twiddle_plant poly4, twiddle1, tmp, q, qa
		mul_twiddle_plant poly5, twiddle2, tmp, q, qa

		vmov twiddle1, s21 
		vmov twiddle2, s22 
		mul_twiddle_plant poly6, twiddle1, tmp, q, qa
		mul_twiddle_plant poly7, twiddle2, tmp, q, qa

		vmov s0, poly0 // a1
		vmov s1, poly1 // a3
		vmov s2, poly2 // a5
		vmov s3, poly3 // a7
		vmov s4, poly4 // a9
		vmov s5, poly5 // a11
		vmov s6, poly6 // a13
		vmov s7, poly7 // a15
	
		// load a0, a2, ..., a14
		load poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
		load poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
		
		// 8-NTT on a0, a2, ..., a14
		_3_layer_double_CT_16_plant_fp poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, s8, s9, s10, s11, s12, s13, s14, twiddle1, twiddle2, q, qa, tmp
		fullplant poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, tmp, q, qa, plantconst
		
		// layer 4 - 1
		// addsub: (a2, a6, a10, a14), (a3, a7, a11, a15)
		vmov twiddle1, s1 // load a3
		uadd16 tmp, poly1, twiddle1
		usub16 poly1, poly1, twiddle1
		str.w tmp, [poly, #1*distance/4]
		str.w poly1, [poly, #1*distance/4+offset]

		vmov twiddle1, s3 // load a7
		uadd16 tmp, poly3, twiddle1
		usub16 poly3, poly3, twiddle1
		str.w tmp, [poly, #3*distance/4]
		str.w poly3, [poly, #3*distance/4+offset]
		
		vmov twiddle1, s5 // load a11
		uadd16 tmp, poly5, twiddle1
		usub16 poly5, poly5, twiddle1
		str.w tmp, [poly, #5*distance/4]
		str.w poly5, [poly, #5*distance/4+offset]
		
		vmov twiddle1, s7 // load a15
		uadd16 tmp, poly7, twiddle1
		usub16 poly7, poly7, twiddle1
		str.w tmp, [poly, #7*distance/4]
		str.w poly7, [poly, #7*distance/4+offset]
		
		// layer 4 - 2    
		// addsub: (a0, a4, a8, a12), (a1, a5, a9, a13)
		vmov poly3, s2 // load a5
		uadd16 tmp, poly2, poly3
		usub16 twiddle1, poly2, poly3
		str.w tmp, [poly, #2*distance/4]
		str.w twiddle1, [poly, #2*distance/4+offset]

		vmov poly5, s4 // load a9
		uadd16 tmp, poly4, poly5
		usub16 twiddle1, poly4, poly5
		str.w tmp, [poly, #4*distance/4]
		str.w twiddle1, [poly, #4*distance/4+offset]

		vmov poly7, s6 // load a13
		uadd16 tmp, poly6, poly7
		usub16 twiddle1, poly6, poly7
		str.w tmp, [poly, #6*distance/4]
		str.w twiddle1, [poly, #6*distance/4+offset]
		
		vmov poly1, s0 // load a1
		uadd16 tmp, poly0, poly1
		usub16 twiddle1, poly0, poly1
		str.w twiddle1, [poly, #offset]
		str.w tmp, [poly], #4
		

		// process 1 coefficient for each 3 coefficients.  

		loadh poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
		loadh poly, poly4, poly5, poly6, poly7, #offset, #distance/4+offset, #2*distance/4+offset, #3*distance/4+offset
		pkhbt_coeffs poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7

		loadh poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
		loadh poly, twiddle1, twiddle2, tmp, qa, #distance+offset, #5*distance/4+offset, #6*distance/4+offset, #7*distance/4+offset
		pkhbt_coeffs poly4, poly5, poly6, poly7, twiddle1, twiddle2, tmp, qa

		// 8-NTT on a1, a3, ..., a15
		movw qa, #30724 
		_3_layer_double_CT_16_plant_fp poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, s8, s9, s10, s11, s12, s13, s14, twiddle1, twiddle2, q, qa, tmp
		fullplant_low poly0, poly1, poly2, poly3, poly4, poly5, poly6, poly7, tmp, q, qa, plantconst

		vmov s0, poly0
		vmov s1, poly1
		vmov s2, poly2
		vmov s3, poly3

		vmov twiddle1, s19 
		vmov twiddle2, s20 
		CS_butterfly_unpack poly4, poly0, twiddle1, tmp, q, qa
		CS_butterfly_unpack poly5, poly1, twiddle2, tmp, q, qa
		
		vmov twiddle1, s21 
		vmov twiddle2, s22  
		CS_butterfly_unpack poly6, poly2, twiddle1, tmp, q, qa
		CS_butterfly_unpack poly7, poly3, twiddle2, tmp, q, qa

		storeh poly, poly4, poly5, poly6, poly7, #distance, #5*distance/4, #6*distance/4, #7*distance/4
		storeh poly, poly0, poly1, poly2, poly3, #distance+offset, #5*distance/4+offset, #6*distance/4+offset, #7*distance/4+offset

		vmov poly0, s0
		vmov poly1, s1
		vmov poly2, s2
		vmov poly3, s3
		
		vmov twiddle1, s15 
		vmov twiddle2, s16 
		CS_butterfly_unpack poly0, poly4, twiddle1, tmp, q, qa
		CS_butterfly_unpack poly1, poly5, twiddle2, tmp, q, qa
		
		vmov twiddle1, s17 
		vmov twiddle2, s18 
		CS_butterfly_unpack poly2, poly6, twiddle1, tmp, q, qa
		CS_butterfly_unpack poly3, poly7, twiddle2, tmp, q, qa

		; storeh poly, poly0, poly1, poly2, poly3, #0, #distance/4, #2*distance/4, #3*distance/4
		storeh poly, poly4, poly5, poly6, poly7, #offset, #distance/4+offset, #2*distance/4+offset, #3*distance/4+offset
		strh poly1, [poly, #distance/4]
		strh poly2, [poly, #2*distance/4]
		strh poly3, [poly, #3*distance/4]
		strh poly0, [poly], #(strincr-4)

			//vmov tmp, s25
			//subs.w tmp, #1
		//bne.w 3b
		vmov tmp, s24
		cmp.w poly, tmp
	bne.w 2b
	vpop.w {s16-s24}
	pop {r4-r11, pc}
