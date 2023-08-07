
#include "macros_smallntt.i"

.syntax unified
.cpu cortex-m4

// CRT(int32_t *out, int16_t *in1, int16_t *in2); output and input buffs cannot be the same
.align 2
.global __asm_small_CRT_32
.type __asm_small_CRT_32, %function
__asm_small_CRT_32:
    push.w {r4-r12, lr}

    des          .req r0
	src1         .req r1
	src2         .req r2
	poly0        .req r3
	poly1        .req r4
	poly2        .req r5
	poly3        .req r6
	tmp          .req r7
	res0         .req r8
	m1           .req r9  // (plant_const* q0^-1 mod q1)*(q1^-1 mod 2^2n) mod 2^2n
	q0           .req r10 
	q1           .req r11 // at the top of r11
	q1a          .req r12 
	// qa=2^a q;a=3; at the bottom of r12
	loop         .req r14

    .equ ldrwidth, 2
    .equ strwidth, 4

    movt q0, #769
    movt q1, #3329
    movw q1a, #26632
    movw m1, #25711
    movt m1, #19686 // m1=(plant_const* q0^-1 mod q1)*(q1^-1 mod 2^2n) mod 2^2n=1290167407

    add.w loop, des, #256*strwidth
    _CRT:
        ldr.w poly0, [src1], #2*ldrwidth
        ldr.w poly1, [src2], #2*ldrwidth
        ldr.w poly2, [src1], #2*ldrwidth
        ldr.w poly3, [src2], #2*ldrwidth

        ssub16.w res0, poly1, poly0
        asr.w poly1, poly0, #16
        sxth.w poly0, poly0
        // mul_twiddle_plant res0, m1, tmp, q1, q1a
        smulwb tmp, m1, res0
        smulwt res0, m1, res0
        smlabt tmp, tmp, q1, q1a
        smlabt res0, res0, q1, q1a
        smlatt poly0, tmp, q0, poly0
        smlatt poly1, res0, q0, poly1

        ssub16.w res0, poly3, poly2
        asr.w poly3, poly2, #16
        sxth.w poly2, poly2
        // mul_twiddle_plant res0, m1, tmp, q1, q1a
        smulwb tmp, m1, res0
        smulwt res0, m1, res0
        smlabt tmp, tmp, q1, q1a
        smlabt res0, res0, q1, q1a
        smlabt poly2, tmp, q0, poly2
        smlatt poly3, res0, q0, poly3

        str.w poly1, [des, #1*strwidth]
        str.w poly2, [des, #2*strwidth]
        str.w poly3, [des, #3*strwidth]
        str.w poly0, [des], #4*strwidth
        
    cmp.w des, loop
    bne.w _CRT

    pop.w {r4-r12, pc}