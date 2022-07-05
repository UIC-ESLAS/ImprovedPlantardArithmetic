/*
 * @Author: Junhao Huang
 * @Date: 2021-09-09 19:09:30
 * @LastEditTime: 2021-10-08 20:41:51
 * @LastEditors: Please set LastEditors
 * @Description: Integrate Plantard Modular reduction
 * @FilePath: \Kyber_M4\kyber\fastbasemul.S
 */

#include "macros.i"
.syntax unified
.cpu cortex-m4
.thumb

// .macro plant_mul1 a, b, q, qa, qinv
// 	smulbb \a, \a, \b
// 	mul \a, \a, \qinv     
// 	//tmp*qinv mod 2^2n/ 2^n; in high half
// 	smlatt \a, \a, \q, \qa
// .endm
// 
// .macro plant_mul2 a, b, tmp, q, qa, qinv
// 	mul \tmp, \qinv, \b // when b is a constant
// 	smulwb \a, \tmp, \a    // we can store b_lo*qinv for latter used 
// 	//tmp*qinv mod 2^2n/ 2^n; in high half
// 	smlatt \a, \a, \q, \qa
// .endm

//q-2= 1 1101 1111 1111
.macro double_fqinv det, tmp, tmp2, loop, q, qa, qinv

	// loop 1
	smulbb \tmp2, \det, \det //tmp2=det^2
	plant_red \q, \qa, \qinv, \tmp2// tmp2 store det^2
	smulbt \tmp, \det, \tmp2 //t=det^3
	plant_red \q, \qa, \qinv, \tmp //result in top half

	// loop 2-8
	movw \loop, #7
	2:
		smultt \tmp2, \tmp2, \tmp2
		plant_red \q, \qa, \qinv, \tmp2//det=det^2
		smultt \tmp, \tmp, \tmp2
		plant_red \q, \qa, \qinv, \tmp// t=t*det
	subs.w \loop, #1
	bne.w 2b
	// loop 9
	smultt \tmp2, \tmp2, \tmp2
	plant_red \q, \qa, \qinv, \tmp2

	// loop 10,11,12
	smultt \tmp2, \tmp2, \tmp2
	plant_red \q, \qa, \qinv, \tmp2
	smultt \tmp, \tmp, \tmp2
	plant_red \q, \qa, \qinv, \tmp

	smultt \tmp2, \tmp2, \tmp2
	plant_red \q, \qa, \qinv, \tmp2
	smultt \tmp, \tmp, \tmp2
	plant_red \q, \qa, \qinv, \tmp

	smultt \tmp2, \tmp2, \tmp2
	plant_red \q, \qa, \qinv, \tmp2
	smultt \tmp2, \tmp, \tmp2
	plant_red \q, \qa, \qinv, \tmp2
	// result in top  half of tmp2.

	// second inversion
	mov \tmp, \det
	smultt \det, \det, \det
	plant_red \q, \qa, \qinv, \det// det= det^2
	smultt \tmp, \tmp, \det //t=t*det
	plant_red \q, \qa, \qinv, \tmp //result in top half

	movw \loop, #7
	3:
		smultt \det, \det, \det
		plant_red \q, \qa, \qinv, \det
		smultt \tmp, \tmp, \det
		plant_red \q, \qa, \qinv, \tmp
	subs.w \loop, #1
	bne.w 3b

	smultt \det, \det, \det
	plant_red \q, \qa, \qinv, \det

	smultt \det, \det, \det
	plant_red \q, \qa, \qinv, \det
	smultt \tmp, \tmp, \det
	plant_red \q, \qa, \qinv, \tmp

	smultt \det, \det, \det
	plant_red \q, \qa, \qinv, \det
	smultt \tmp, \tmp, \det
	plant_red \q, \qa, \qinv, \tmp

	smultt \det, \det, \det
	plant_red \q, \qa, \qinv, \det
	smultt \det, \tmp, \det
	plant_red \q, \qa, \qinv, \det
	// result in top  half of det.
	pkhtb \det, \det, \tmp2, asr#16
.endm

// double 3-degree baseinv: six polynomial in 3 poly
.macro double_baseinv rptr, b01, b20, b12, a01, a20, a12, zeta, tmp, tmp2, q, qa, qinv
	//a0 a1 computation
	smulbb \b01, \a01, \a01
	plant_red \q, \qa, \qinv, \b01// top half of tmp

	smulbb \tmp, \a20, \a20
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp
	smlabt \tmp, \tmp, \q, \qa
	pkhtb \b01, \tmp, \b01, asr#16 //b01: two coeffs
	
	smultb \tmp, \a01, \a20
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp
	smlabt \tmp, \tmp, \q, \qa// t=fqmul(t, zeta)
	smulbt \tmp2, \a01, \a01
	plant_red \q, \qa, \qinv, \tmp2
	pkhtb \tmp, \tmp2, \tmp, asr#16
	usub16 \b01, \b01, \tmp // b0|b1-t0|t1

	//a2 a0 computation
	smultt \b20, \a01, \a01
	plant_red \q, \qa, \qinv, \b20 //a1*a1 in top half of b20
	smultt \tmp, \a20, \a20
	plant_red \q, \qa, \qinv, \tmp// a3*a3 in top half of tmp
	pkhtb \b20, \tmp, \b20, asr#16

	mov \b12, \zeta
	neg \zeta, \zeta//-zeta

	smulbb \tmp, \a01, \a20
	plant_red \q, \qa, \qinv, \tmp//t2=a0*a2
	smulbt \tmp2, \a12, \a12
	plant_red \q, \qa, \qinv, \tmp2//a4*a5
	smulwt \tmp2, \zeta, \tmp2
	smlabt \tmp2, \tmp2, \q, \qa// t=fqmul(t, zeta)
	pkhtb \tmp, \tmp2, \tmp, asr#16
	usub16 \b20, \b20, \tmp // b2|b3-t2|t3
	
	//det0 computation needs original deta: b12
	pkhtb \tmp, \b01, \b20
	pkhtb \tmp2, \a01, \a20 //b1|b2 a1|a2
	smuadx \tmp, \tmp, \tmp2
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \b12, \tmp 
	smlabt \tmp, \tmp, \q, \qa //det=det*zeta in top half
	smulbb \tmp2, \b01, \a01
	plant_red \q, \qa, \qinv, \tmp2 //a0*b0
	uadd16 \a01, \tmp, \tmp2
	//det0 in a01 top half

	//a1, a2 computation
	smultt \b12, \a12, \a12
	plant_red \q, \qa, \qinv, \b12
	smulwt \b12, \zeta, \b12
	smlabt \b12, \b12, \q, \qa

	smulbb \tmp, \a12, \a12
	plant_red \q, \qa, \qinv, \tmp //a1*a1 in top half of tmp
	pkhtb \b12, \tmp, \b12, asr#16 //b12: two coeffs

	// t1, t2 computation
	smultb \tmp, \a20, \a12
	plant_red \q, \qa, \qinv, \tmp//t1=a0*a1
	smultt \tmp2, \a20, \a12
	plant_red \q, \qa, \qinv, \tmp2//t2=a0*a2
	pkhtb \tmp, \tmp2, \tmp, asr#16
	usub16 \b12, \b12, \tmp

	//det1
	smuadx \tmp, \b12, \a12
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp 
	smlabt \tmp, \tmp, \q, \qa //det=det*zeta in top half
	smultt \tmp2, \b20, \a20
	plant_red \q, \qa, \qinv, \tmp2 //a0*b0
	uadd16 \tmp, \tmp, \tmp2
	//det1 in tmp top half
	pkhtb \a01, \tmp, \a01, asr#16

	double_fqinv \a01, \tmp, \tmp2, \a20, \q, \qa, \qinv//a01=fqinv(det0|det1)
	//movw \a01, #1
	//movt \a01, #1
	smulbb \tmp, \b01, \a01
	plant_red \q, \qa, \qinv, \tmp
	smultb \tmp2, \b01, \a01
	plant_red \q, \qa, \qinv, \tmp2
	pkhtb \b01, \tmp2, \tmp, asr#16

	smulbb \tmp, \b20, \a01
	plant_red \q, \qa, \qinv, \tmp
	smultt \tmp2, \b20, \a01
	plant_red \q, \qa, \qinv, \tmp2
	pkhtb \b20, \tmp2, \tmp, asr#16

	smulbt \tmp, \b12, \a01
	plant_red \q, \qa, \qinv, \tmp
	smultt \tmp2, \b12, \a01
	plant_red \q, \qa, \qinv, \tmp2
	pkhtb \b12, \tmp2, \tmp, asr#16

	str.w \b01, [\rptr], #4
	str.w \b20, [\rptr], #4
	str.w \b12, [\rptr], #4

	//r = (uint16_t)det;
	//r = (uint32_t)(-r) >> 31;
	//return r - 1;
	lsr \tmp2, \a01, #16
	uxth \tmp, \a01	
	uxth \tmp2, \tmp2
	neg \tmp, \tmp
	neg \tmp2, \tmp2
	lsr \tmp, #31
	add \b01, \tmp, \tmp2, lsr#31
	sub \b01, \b01, #2
	//return value in \b01
.endm

// double 3-degree basemul: six polynomial in 3 uint32_t;
.macro double_basemul rptr, res, a01, a20, a12, b01, b20, b12, zeta, tmp, tmp2, q, qa, qinv
	//c01
	pkhtb \tmp, \a01, \a20 //a1|a2
	pkhtb \tmp2, \b01, \b20 //b1|b2
	smuadx \tmp, \tmp, \tmp2 //a1*b2+a2*b1
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp //c0*zeta*qinv
	smulbb \tmp2, \a01, \b01 //a0*b0
	mul \tmp2, \tmp2, \qinv 
	add \res, \tmp, \tmp2, lsr#16//c0*zeta*qinv+a0*b0*qinv
	smlabt \res, \res, \q, \qa
	// c0
	
	smulbb \tmp, \a20, \b20
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp//c1*zeta*qinv
	smuadx \tmp2, \a01, \b01
	mul \tmp2, \tmp2, \qinv
	add \tmp, \tmp, \tmp2, lsr#16//c1*zeta*qinv+(a0*b1+a1*b0)*qinv
	smlabt \tmp, \tmp, \q, \qa
	pkhtb \res, \tmp, \res, asr#16
	str.w \res, [\rptr], #4
	// c1 & c01

	pkhbt \tmp, \a01, \a20, lsl#16 //a0|a2
	pkhbt \tmp2, \b01, \b20, lsl#16 //b0|b2
	smuadx \tmp, \tmp, \tmp2
	smlatt \res, \a01, \b01, \tmp
	plant_red \q, \qa, \qinv, \res
	// c2
	neg \zeta, \zeta
	smuadx \tmp, \a12, \b12 //a1*b2+a2*b1
	plant_red \q, \qa, \qinv, \tmp
	smulwt \tmp, \zeta, \tmp //c0*zeta*qinv
	smultt \tmp2, \a20, \b20 //a0*b0
	mul \tmp2, \tmp2, \qinv 
	add \tmp, \tmp, \tmp2, lsr#16//c0*zeta*qinv+a0*b0*qinv
	smlabt \tmp, \tmp, \q, \qa
	pkhtb \res, \tmp, \res, asr#16
	str.w \res, [\rptr], #4
	// c0 &c20

	smultt \tmp, \a12, \b12
	plant_red \q, \qa, \qinv, \tmp
	smulwt \res, \zeta, \tmp//c1*zeta*qinv
	pkhtb \tmp, \a20, \a12 //a1|a0
	pkhtb \tmp2, \b20, \b12 //b1|b0
	smuadx \tmp2, \tmp, \tmp2
	mul \tmp2, \tmp2, \qinv
	add \res, \res, \tmp2, lsr#16//c1*zeta*qinv+(a0*b1+a1*b0)*qinv
	smlabt \res, \res, \q, \qa
	// c1

	pkhtb \tmp, \a20, \a12, asr#16 //a0|a2
	pkhtb \tmp2, \b20, \b12, asr#16 //b0|b2
	smuadx \tmp, \tmp, \tmp2
	smlabb \tmp, \a12, \b12, \tmp
	plant_red \q, \qa, \qinv, \tmp
	pkhtb \res, \tmp, \res, asr#16
	str.w \res, [\rptr], #4
	// c2 && c12
.endm


.global poly_baseinv_asm
.type poly_baseinv_asm, %function
.align 2
poly_baseinv_asm:
	push {r4-r11, lr}

	rptr  .req r0
	poly  .req r1
	zetaptr .req r2
	poly0 .req r3
	poly1 .req r4
	poly2 .req r5
	res0  .req r6
	res1  .req r7
	res2  .req r8
	q     .req r9
	qa    .req r10
	qinv  .req r11
	tmp   .req r12
	loop  .req r14
	zeta  .req r14

	movw qa, #30724
	movt  q, #7681
	### qinv=0x8b83e201
	movw qinv, #57857
	movt qinv, #35715

	movw loop, #128
	movw tmp, #0
	1:
		vmov s0, loop
		vmov s1, tmp

		ldm poly!,{poly0-poly2}
		ldr.w zeta, [zetaptr], #4

		vmov s2, zetaptr
		//zetaptr as tmp2; the return value stored in res0;
		double_baseinv rptr, res0, res1, res2, poly0, poly1, poly2, zeta, tmp, zetaptr, q, qa, qinv

		vmov loop, s0
		vmov tmp, s1
		vmov zetaptr, s2
		adds.w tmp, tmp, res0
	subs.w loop, #1
	bne.w 1b

	mov r0, tmp// return value
	pop {r4-r11, pc}

	.unreq zetaptr
	.unreq poly0 
	.unreq poly1 
	.unreq poly2 
	.unreq res0  
	.unreq res1  
	.unreq res2  
	.unreq q     
	.unreq qa    
	.unreq qinv  
	.unreq tmp   
	.unreq loop  
	.unreq zeta  

// output -0.5p-0.5p
.global poly_basemul_asm
.type poly_basemul_asm, %function
.align 2
poly_basemul_asm:
	push {r4-r11, lr}

	rptr  .req r0
	aptr  .req r1
	bptr  .req r2
	zetaptr .req r3
	a01    .req r4
	a20    .req r5
	a12    .req r6
	b01    .req r7
	b20    .req r8
	b12    .req r9
	q     .req r10
	qa    .req r11
	qinv  .req r12
	tmp   .req r14
	zeta  .req r1 //reuse aptr

	movw qa, #30724
	movt  q, #7681
	### qinv=0x8b83e201
	movw qinv, #57857
	movt qinv, #35715

	movw tmp, #128
	1:
		ldm aptr!,{a01-a12}
		ldm bptr!,{b01-b12}

		vmov s0, aptr

		ldr.w zeta, [zetaptr], #4

		vmov s1, zetaptr
		vmov s2, tmp
		vmov s3, bptr
		//bptr as res, zetaptr as tmp2
		double_basemul rptr, bptr, a01, a20, a12, b01, b20, b12, zeta, tmp, zetaptr, q, qa, qinv

		vmov aptr,s0
		vmov zetaptr, s1
		vmov tmp, s2		
		vmov bptr,s3		
	subs.w tmp, #1
	bne.w 1b
	pop {r4-r11, pc}