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

.syntax unified
.cpu cortex-m4
.thumb

.macro plant_red q, qa, qinv, tmp
	mul \tmp, \tmp, \qinv     
	//tmp*qinv mod 2^2n/ 2^n; in high half
	smlatt \tmp, \tmp, \q, \qa
	// result in high half
.endm

// output -0.5p, 0.5p
.global doublebasemul_asm
.type doublebasemul_asm, %function
.align 2
doublebasemul_asm:
	push {r4-r11, lr}

	rptr  .req r0
	aptr  .req r1
	bptr  .req r2
	zeta  .req r3 
	// zeta=q_inv*((zeta*plant_const) mod q) mod 2^2n
	poly0 .req r4
	poly1 .req r6
	poly2 .req r5
	poly3 .req r7
	q     .req r8
	qa    .req r9
	qinv  .req r10
	tmp   .req r11
	tmp2  .req r12

	### q in the top half
	movw qa, #26632
	movt  q, #3329  
	### qinv=0x6ba8f301
	movw qinv, #62209
	movt qinv, #27560

	ldrd poly0, poly2, [aptr], #8
	ldrd poly1, poly3, [bptr], #8

	//basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
	smulwt tmp, zeta, poly1 
	// b_1*zeta*qinv*plant_const; in low half
	smlabt tmp, tmp, q, qa  
	// b_1*zeta
	smultt tmp, poly0, tmp  
	//a_1*b_1*zeta <2^32
	smlabb tmp, poly0, poly1, tmp 
	// a1*b1*zeta+a0*b0
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly0, poly1 
	plant_red q, qa, qinv, tmp2

	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	str tmp, [rptr], #4

	neg zeta, zeta

	//basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
	smulwt tmp, zeta, poly3 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly2, tmp  
	smlabb tmp, poly2, poly3, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly2, poly3 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	str tmp, [rptr], #4

	.unreq rptr  
	.unreq aptr  
	.unreq bptr  
	.unreq zeta  
	.unreq poly0 
	.unreq poly1 
	.unreq poly2 
	.unreq poly3 
	.unreq q     
	.unreq qa    
	.unreq qinv  
	.unreq tmp   
	.unreq tmp2  

	pop {r4-r11, pc}

// output -0.5p-0.5p
.global doublebasemul_asm_acc
.type doublebasemul_asm_acc, %function
.align 2
doublebasemul_asm_acc:
	push {r4-r11, lr}

	rptr  .req r0
	aptr  .req r1
	bptr  .req r2
	zeta  .req r3
	poly0 .req r4
	poly1 .req r6
	poly2 .req r5
	poly3 .req r7
	q     .req r8
	qa    .req r14
	qinv  .req r9
	tmp   .req r10
	tmp2  .req r11
	r0r1  .req r12
	// r2r3  .req r14

	movw qa, #26632
	movt  q, #3329
	### qinv=0x6ba8f301
	movw qinv, #62209
	movt qinv, #27560

	ldrd poly0, poly2, [aptr], #8
	ldrd poly1, poly3, [bptr], #8

	ldr.w r0r1, [rptr]
	//basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
	smulwt tmp, zeta, poly1 
	smlabt tmp, tmp, q, qa 
	smultt tmp, poly0, tmp 
	smlabb tmp, poly0, poly1, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly0, poly1 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	uadd16 r0r1, r0r1, tmp
	str.w r0r1, [rptr], #4

	neg zeta, zeta

	ldr.w r0r1, [rptr]
	//basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
	smulwt tmp, zeta, poly3 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly2, tmp  
	smlabb tmp, poly2, poly3, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly2, poly3 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	uadd16 r0r1, r0r1, tmp
	str r0r1, [rptr], #4

	.unreq rptr  
	.unreq aptr  
	.unreq bptr  
	.unreq zeta  
	.unreq poly0 
	.unreq poly1 
	.unreq poly2 
	.unreq poly3 
	.unreq q     
	.unreq qa    
	.unreq qinv  
	.unreq tmp   
	.unreq tmp2  

	.unreq r0r1  
	//.unreq r2r3  

	pop {r4-r11, pc}

// -0.5p~0.5p
.global basemul_asm
.type basemul_asm, %function
.align 2
basemul_asm:
	push {r4-r11, lr}

	rptr    .req r0
	aptr    .req r1
	bptr    .req r2
	zetaptr .req r3
	poly0   .req r4
	poly1   .req r6
	poly2   .req r5
	poly3   .req r7
	q       .req r8
	qa      .req r14
	qinv    .req r9
	tmp     .req r10
	tmp2    .req r11
	zeta    .req r12
	loop    .req r14

	//movw qa, #26632
	movt  q, #3329
	### qinv=0x6ba8f301
	movw qinv, #62209
	movt qinv, #27560

#ifdef USE_REPT
	movw qa, #26632
	.rept 64
#else
	movw loop, #64
	1:
	vmov.w s0,loop
	movw qa, #26632
#endif
			
	ldrd poly0, poly2, [aptr], #8
	ldrd poly1, poly3, [bptr], #8 
	// ldr poly0, [aptr], #4
	// ldr poly1, [bptr], #4
	// ldr poly2, [aptr], #4
	// ldr poly3, [bptr], #4

	ldr.w zeta, [zetaptr], #4

	// basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
	smulwt tmp, zeta, poly1 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly0, tmp  
	smlabb tmp, poly0, poly1, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly0, poly1 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	str tmp, [rptr], #4

	neg zeta, zeta

	// basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
	smulwt tmp, zeta, poly3 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly2, tmp  
	smlabb tmp, poly2, poly3, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly2, poly3 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	str tmp, [rptr], #4
		
#ifdef USE_REPT
	.endr
#else
	vmov.w loop,s0
	subs.w loop, #1
	bne.w 1b
#endif

	.unreq rptr   
	.unreq aptr   
	.unreq bptr   
	.unreq zetaptr
	.unreq poly0  
	.unreq poly1  
	.unreq poly2  
	.unreq poly3  
	.unreq q      
	.unreq qa     
	.unreq qinv   
	.unreq tmp    
	.unreq tmp2   
	.unreq zeta   
	.unreq loop   

	pop {r4-r11, pc}
//-0.5p~0.5p
.global basemul_asm_acc
.type basemul_asm_acc, %function
.align 2
basemul_asm_acc:
	push {r4-r11, lr}

	rptr    .req r0
	aptr    .req r1
	bptr    .req r2
	zetaptr .req r3
	poly0   .req r4
	poly1   .req r6
	poly2   .req r5
	poly3   .req r7
	q       .req r8
	qa      .req r14
	qinv    .req r9
	tmp     .req r10
	tmp2    .req r11
	zeta    .req r12
	loop    .req r14

	
	movt  q, #3329
	### qinv=0x6ba8f301
	movw qinv, #62209
	movt qinv, #27560
#ifdef USE_REPT
	movw qa, #26632
	.rept 64
#else
	movw loop, #64
	1:
		vmov.w s0,loop
		movw qa, #26632
#endif

	ldrd poly0, poly2, [aptr], #8
	ldrd poly1, poly3, [bptr], #8

	ldr.w zeta, [zetaptr], #4

	//basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
	smulwt tmp, zeta, poly1 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly0, tmp  
	smlabb tmp, poly0, poly1, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly0, poly1 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	
	ldr.w tmp2, [rptr]
	uadd16 tmp, tmp, tmp2
	str.w tmp, [rptr], #4

	neg zeta, zeta

	// basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
	smulwt tmp, zeta, poly3 
	smlabt tmp, tmp, q, qa  
	smultt tmp, poly2, tmp  
	smlabb tmp, poly2, poly3, tmp 
	plant_red q, qa, qinv, tmp
	// r[0] in upper half of tmp
	
	smuadx tmp2, poly2, poly3 
	plant_red q, qa, qinv, tmp2
	// r[1] in upper half of tmp2
	pkhtb tmp, tmp2, tmp, asr#16
	
	ldr.w tmp2, [rptr]
	uadd16 tmp, tmp, tmp2
	str.w tmp, [rptr], #4
#ifdef USE_REPT
	.endr
#else
	vmov.w loop, s0
	subs.w loop, #1
	bne.w 1b
#endif

	.unreq rptr    
	.unreq aptr    
	.unreq bptr    
	.unreq zetaptr 
	.unreq poly0   
	.unreq poly1   
	.unreq poly2   
	.unreq poly3   
	.unreq q       
	.unreq qa      
	.unreq qinv    
	.unreq tmp     
	.unreq tmp2    
	.unreq zeta    
	.unreq loop    

	pop {r4-r11, pc}
