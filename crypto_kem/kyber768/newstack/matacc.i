.macro plant_red q, qa, qinv, tmp
	mul \tmp, \tmp, \qinv     
	//tmp*qinv mod 2^2n/ 2^n; in high half
	smlatt \tmp, \tmp, \q, \qa
	// result in high half
.endm

// Checks if val0 is suitable and multiplies with values from bptr using func 
.macro first_if func, tmp, tmp2, val0, val1, rptr, bptr, cptr, bufptr, zetaptr, k, q, qa, qinv, ctr
// if (val0 < KYBER_Q)
    cmp.w \val0, \q
    bhs.w 2f
        strh \val0, [\cptr], #2
        add \k, #1
        cmp.w \k, #4
        bne.w 2f
            sub \cptr, #4*2
            vmov s2, \bufptr
            vmov s4, \ctr
            vmov s5, \val1
            \func \rptr, \bptr, \cptr, \zetaptr, \bufptr, \k, \val0, \val1, \q, \qa, \qinv, \tmp, \tmp2, \ctr
            vmov \bufptr, s2
            vmov \ctr, s4
            vmov \val1, s5

            add ctr, #1
            
            movw k, #0
    2:
.endm

// Checks if val1 is suitable and multiplies with values from bptr using func 
.macro second_if func, tmp, tmp2, val0, val1, rptr, bptr, cptr, bufptr, zetaptr, k, q, qa, qinv, ctr
// if (val1 < KYBER_Q && ctr < KYBER_N/4)
    cmp.w \val1, \q
    bhs.w 2f
        cmp.w \ctr, #256/4
        bge.w 2f
            strh \val1, [\cptr], #2
            add \k, #1
            cmp.w \k, #4
            bne.w 2f
                sub \cptr, #4*2
                vmov s2, \bufptr
                vmov s4, \ctr
                \func \rptr, \bptr, \cptr, \zetaptr, \bufptr, \k, \val0, \val1, \q, \qa, \qinv, \tmp, \tmp2, \ctr
                vmov \bufptr, s2
                vmov \ctr, s4

                add \ctr, #1
                
                movw \k, #0
    2:
.endm

.macro third_if tmp, tmp2, rptr, bptr, cptr, bufptr, ctr
// if (pos + 3 > buflen && ctr < KYBER_N/4)
  vmov \tmp, s1
  add \tmp, #168 // XOF_BLOCKBYTES=168
  add \tmp2, \bufptr, #3
  cmp.w \tmp2, \tmp  // pos + 3 > buflen
  ble.w 2f
    cmp.w \ctr, #256/4
    bge.w 2f
      vmov \bufptr, s1

      vmov s2, \rptr
      vmov s3, \bptr
      vmov s4, \cptr
      vmov s5, \ctr

      mov \rptr, \bufptr
      movw \bptr, #1
      vmov \cptr, s10 // load state
      #ifndef nohash
      bl kyber_shake128_squeezeblocks
      #endif
      
      vmov \rptr, s2
      vmov \bptr, s3
      vmov \cptr, s4
      vmov \ctr, s5
      vmov \bufptr, s1
    2:
.endm

.macro doublebasemul_asm rptr, aptr, bptr, zetaptr, poly0, poly1, poly2, poly3, q, qa, qinv, tmp, tmp2, zeta
    ldr \poly0, [\aptr], #4
    ldr \poly1, [\bptr]
    ldr \poly2, [\aptr], #4
    ldr.w \poly3, [\bptr, #4]
    ldr \zeta, [\zetaptr], #4

    //basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
    smulwt \tmp, \zeta, \poly1 
    // b_1*zeta*qinv*plant_const; in low half
    smlabt \tmp, \tmp, \q, \qa  
    // b_1*zeta
    smultt \tmp, \poly0, \tmp  
    //a_1*b_1*zeta <2^32
    smlabb \tmp, \poly0, \poly1, \tmp 
    // a1*b1*zeta+a0*b0
    plant_red \q, \qa, \qinv, \tmp
    // r[0] in upper half of tmp
    smuadx \tmp2, \poly0, \poly1 
    plant_red \q, \qa, \qinv, \tmp2
    // r[1] in upper half of tmp2
    pkhtb \tmp, \tmp2, \tmp, asr#16
    str \tmp, [\rptr], #4

    neg \zeta, \zeta

    //basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
    smulwt \tmp, \zeta, \poly3 
    smlabt \tmp, \tmp, \q, \qa  
    smultt \tmp, \poly2, \tmp  
    smlabb \tmp, \poly2, \poly3, \tmp 
    plant_red \q, \qa, \qinv, \tmp
    // r[0] in upper half of tmp
    
    smuadx \tmp2, \poly2, \poly3 
    plant_red \q, \qa, \qinv, \tmp2
    // r[1] in upper half of tmp2
    pkhtb \tmp, \tmp2, \tmp, asr#16
    str \tmp, [\rptr], #4
.endm
// res replace poly2
.macro doublebasemul_asm_acc rptr, aptr, bptr, zetaptr, poly0, poly1, res, poly3, q, qa, qinv, tmp, tmp2, zeta
    ldr \poly0, [\aptr], #4
    ldr \poly1, [\bptr]
    ldr.w \poly3, [\bptr, #4]
    ldr.w \res, [\rptr]
    ldrh \zeta, [\zetaptr], #2

    //basemul(r->coeffs + 4 * i, a->coeffs + 4 * i, b->coeffs + 4 * i, zetas[64 + i]);
    smulwt \tmp, \zeta, \poly1 
    // b_1*zeta*qinv*plant_const; in low half
    smlabt \tmp, \tmp, \q, \qa  
    // b_1*zeta
    smultt \tmp, \poly0, \tmp  
    //a_1*b_1*zeta <2^32
    smlabb \tmp, \poly0, \poly1, \tmp 
    // a1*b1*zeta+a0*b0
    plant_red \q, \qa, \qinv, \tmp
    // r[0] in upper half of tmp
    smuadx \tmp2, \poly0, \poly1 
    plant_red \q, \qa, \qinv, \tmp2
    // r[1] in upper half of tmp2
    pkhtb \tmp, \tmp2, \tmp, asr#16
    uadd16 \res, \res, \tmp
    str \res, [\rptr], #4

    neg \zeta, \zeta

    ldr.w \res, [\rptr]
    ldr \poly0, [\aptr], #4
    //basemul(r->coeffs + 4 * i + 2, a->coeffs + 4 * i + 2, b->coeffs + 4 * i + 2, - zetas[64 + i]);
    smulwt \tmp, \zeta, \poly3 
    smlabt \tmp, \tmp, \q, \qa  
    smultt \tmp, \poly0, \tmp  
    smlabb \tmp, \poly0, \poly3, \tmp 
    plant_red \q, \qa, \qinv, \tmp
    // r[0] in upper half of tmp
    
    smuadx \tmp2, \poly0, \poly3 
    plant_red \q, \qa, \qinv, \tmp2
    // r[1] in upper half of tmp2
    pkhtb \tmp, \tmp2, \tmp, asr#16
    uadd16 \res, \res, \tmp
    str \res, [\rptr], #4
.endm