;-gcd_stein.asm------------------------------------------------------------------------------------;
;~  description         ACAMA GCD algorithm assembler testing.

;~  history             Christian Host-Madsen       2026.09.06      creation

;*  global fns          gcd_stein                   Stein's GCD algorithm and program entry
;*  local fns           none

;*  global vars         a_l                         Gcd number a [l]
;*                      a_h                         Gcd number a [h]
;*                      b_l                         Gcd number b [l]
;*                      b_h                         Gcd number b [h]

; constants
.const      LOW_BIT_MASK            0x01                            ; low bit check mask

;-CODE-SEGMENT-------------------------------------------------------------------------------------;
.code
.org        0x0000

;-gcd_stein----------------------------------------------------------------------------------------;
;~  description         Finds the GCD of two words using Stein's algorithm (and program entry).
;~  operation           Uses Stein's GCD algorithm to find the GCD of unsigned 16-bit numbers a and
;                       b, then return with the high GCD in X and low in the accumulator.

;~  arguments           none
;~  return              uint16              gcd of a and b                          * X | A

;~  local variables     uint8   k           stein k counter
;~  shared variables    none
;~  global variables    uint16  a_l         number a [l]
;~                      uint8   a_h         number a [h]
;~                      uint16  b_l         number b [l]
;~                      uint8   b_h         number b [h]

;~  input               none
;~  output              none
 
;~  error handling      none
;~  algorithms          Stein's GCD algorithm.
;~  data structures     none

;*  last modified       Christian Host-Madsen       2026.09.06      creation

;*  stack depth         0
gcd_stein:
    ldd     a_l                                                     ; check for nonzero a
    or      a_h
    jnz     a_nz
  ; jz      ret_b
  ; TODO : nop?

    ret_b:      ldd     b_h                                         ; a is zero, return b
                tax
                ldd     b_l
                rts
                nop                                                 ; [*] branch delay

    a_nz:       ldd     b_l                                         ; check for nonzero b
                or      b_l
                jnz     set_k
              ; jz      ret_a
              ; TODO : nop?

    ret_a:      ldd     b_h                                         ; a is zero, return b
                tax
                ldd     b_l
                rts
                nop                                                 ; [*] branch delay

    set_k:      ldi     0x00                                        ; X to initialize k
                tax
                ldd     a_l                                         ; [*] a_l preload

    pair_shft:  or      b_l                                         ; check if a or b is odd
                andi    LOW_BIT_MASK
                jnz     store_k

                ldd     a_h                                         ; shift a
                lsr
                std     a_h
                ldd     a_l
                rrc
                std     a_l

                ldd     b_h                                         ; shift b
                lsr
                std     b_h
                ldd     b_l
                rrc
                std     b_l

                inx                                                 ; increment X (k)
                jmp     pair_shft
                ldd     a_l                                         ; [*] a_l preload

    store_k:    txa                                                 ; store k as X
                std     k
                ldd     a_l                                         ; [*] a_l preload

    find_gcd:   andi    LOW_BIT_MASK                                ; check if a is odd
                jnz     a_odd      

                ldd     a_h                                         ; shift a
                lsr
                ldd     a_l
                rrc

                jmp     find_gcd                                    ; [*] aka make_a_odd
                ldd     a_l                                         ; [*] a_l preload

    a_odd:      ldd     a_l                                         ; check if b > a
                sub     b_l
                ldd     a_h
                sbb     b_h

                jc      a_geq_b
              ; jnc     swap_words

    swap_words: ldd     a_l                                         ; move a low to X
                tax
                ldd     b_l                                         ; move b low to a low
                std     a_l
                tax                                                 ; move X to b low
                std     b_l

                ldd     a_h                                         ; move a high to X
                tax
                ldd     b_h                                         ; move b high to a low
                std     a_h
                tax                                                 ; move X to b high
                std     b_h

    a_geq_b:  ; ldd     a_l                                         ; [*] a preloaded
                sub     b_l                                         ; adjust a
                std     a_l
                ldd     a_h
                sbb     b_h
                std     a_h

                ldi     a_l                                         ; check if a is zero
                or      a_h
                jz      k_load
                ldd     k                                           ; [*] preload k && branch delay
                jmp     find_gcd

    k_load:     ldd     k                                           ; check if k is zero
                ori     0x00
                jnz     get_gcd                                     ; [*] aka k_nz
              ; jz      k_zero

    k_zero:     ldd     b_h                                         ; k is zero, return b
                tax
                ldd     b_l
                ret
                nop
    
    get_gcd:    ldd     b_l                                         ; shift b
                lsl
                std     b_l
                ldd     b_h
                rlc
                std     b_h

                ldd     k                                           ; decrement k until zero
                dec
                jnz     get_gcd
              ; jz      got_gcd
                std     k                                           ; [*] store k && branch delay
    
    got_gcd:    ldd     b_h                                         ; return gcd (b)
                tax
                ldd     b_l
                ret
                nop


;-DATA-SEGMENT-------------------------------------------------------------------------------------;
.data
.org        0x0000

; gcd arguments
a_l                 db          ?                                   ; number a [l]
a_h                 db          ?                                   ; number a [h]
b_l                 db          ?                                   ; number b [l]
b_h                 db          ?                                   ; number b [h]

; gcd local variables
k                   db          ?                                   ; number k
