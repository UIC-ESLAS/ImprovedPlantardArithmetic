/*
This file contains wrappers for assembly code that is not callable via a function from the regular sources.
*/
.syntax unified
.cpu cortex-m4
.thumb

#include "implvariant.h"

#ifdef optstack
#include "matacc.i"

.global doublebasemul_asm_wrapper
.type doublebasemul_asm_wrapper, %function
.align 2
doublebasemul_asm_wrapper:
push {r0-r11, r14}
doublebasemul_asm r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r14
pop {r0-r11, pc}
.size doublebasemul_asm_wrapper, . - doublebasemul_asm_wrapper

.global doublebasemul_asm_acc_wrapper
.type doublebasemul_asm_acc_wrapper, %function
.align 2
doublebasemul_asm_acc_wrapper:
push {r0-r11, r14}
doublebasemul_asm_acc r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_wrapper, . - doublebasemul_asm_acc_wrapper

#endif