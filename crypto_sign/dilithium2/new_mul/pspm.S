.syntax unified
.cpu cortex-m4

// input: int32_t* r, int32_t* v, int16_t* c, uint8_t* c_idx, int16_t* s. 
// PSPM with operand scanning
.align 2
.global __asm_pspm_os
.type __asm_pspm_os, %function
__asm_pspm_os:
  push.w {r0-r11, lr}
  rptr      .req r0
  vptr      .req r1
  cptr      .req r2
  cidxptr   .req r3
  sptr      .req r4 
  loop      .req r5
  c_0        .req r6
  val0      .req r7
  val1      .req r8
  val2      .req r9
  res0      .req r10
  res1      .req r11
  res2      .req r12
  k         .req r14

  .equ tau, 39
  .equ distance, 256

  ldr.w sptr, [sp, #13*4] // load s addr
  add.w loop, cidxptr, #tau  // TAU= 39, 49, or 60 
  vmov s1, loop
  vmov s2, vptr
  1:
    ldrb k, [cidxptr], #1  // first index non-zero in c
    ldrsh c_0, [cptr, k, lsl #1]   // val0=c[k]

    // if c_0==1
    cmp c_0, #1
    bne.w second_if
    first_if:
      add.w loop, sptr, #(distance-1)*2
      add.w vptr, vptr, k, lsl #2  //v[k]
      2: 
        ldrsh val0, [sptr], #2
        ldrsh val1, [sptr], #2
        ldrsh val2, [sptr], #2
        ldr.w res0, [vptr, #0]
        ldr.w res1, [vptr, #4]
        ldr.w res2, [vptr, #8]

        add.w res0, res0, val0
        add.w res1, res1, val1
        add.w res2, res2, val2

        str.w res0, [vptr], #4
        str.w res1, [vptr], #4
        str.w res2, [vptr], #4

      cmp sptr, loop
      bne.w 2b

      ldrsh val0, [sptr], #2
      ldr.w res0, [vptr, #0]
      add.w res0, res0, val0
      str.w res0, [vptr], #4
      b end_if
    // if c_0==-1
    second_if:
      add.w loop, sptr, #(distance-1)*2
      add.w vptr, vptr, k, lsl #2  //v[k]
      3: 
        ldrsh val0, [sptr], #2
        ldrsh val1, [sptr], #2
        ldrsh val2, [sptr], #2
        ldr.w res0, [vptr, #0]
        ldr.w res1, [vptr, #4]
        ldr.w res2, [vptr, #8]

        sub.w res0, res0, val0
        sub.w res1, res1, val1
        sub.w res2, res2, val2

        str.w res0, [vptr], #4
        str.w res1, [vptr], #4
        str.w res2, [vptr], #4

      cmp sptr, loop
      bne.w 3b

      ldrsh val0, [sptr], #2
      ldr.w res0, [vptr, #0]
      sub.w res0, res0, val0
      str.w res0, [vptr], #4
    end_if:
  sub.w sptr, #distance*2
  vmov vptr, s2
  vmov loop, s1
  cmp cidxptr, loop
  bne.w 1b
  
  add.w loop, rptr, #distance*4
  4:
    ldr.w val0, [vptr], #4
    ldr.w val1, [vptr], #4
    ldr.w val2, [vptr], #4
    ldr.w k, [vptr], #4
    ldr.w res0, [rptr, #0]
    ldr.w res1, [rptr, #4]
    ldr.w res2, [rptr, #8]
    ldr.w c_0, [rptr, #12]

    sub.w res0, res0, val0
    sub.w res1, res1, val1
    sub.w res2, res2, val2

    str.w res0, [vptr], #4
    str.w res1, [vptr], #4
    str.w res2, [vptr], #4
  cmp rptr, loop
  bne.w 4b

  pop.w {r0-r11, pc}