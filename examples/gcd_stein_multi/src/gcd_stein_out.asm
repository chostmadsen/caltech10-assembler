;-gcd_stein_out.asm----------------------------------------------------------------------------------------------------;
;~  description         Caltech10 assembler GCD algorithm assembler testing (multi-file test).
;~                      Simulates outputting the gcd at port PORT_OUT.

;~  history             Christian Host-Madsen       2026.09.06      creation
;~                      Christian Host-Madsen       2026.09.11      multi-file split

;*  global fns          gcd_out                     Test GCD Stein output.
;*  local fns           none

;*  local vars          tmp_store                   temporary storage


; inclusions (echoed)
; .include    "gcd_stein.asm"


;-CODE-SEGMENT---------------------------------------------------------------------------------------------------------;
.code

;-gcd_stein_out------------------------------------------------------------------------------------;
;~  description         Outputs whatever the gcd is to PORT_OUT.
;~  operation           uses the `out` instruction to output the GCD.

;~  arguments           uint16              gcd of a and b                          * X | A
;~  return              none

;~  local variables     none
;~  shared variables    none
;~  global variables    none

;~  input               none
;~  output              port PORT_OUT
 
;~  error handling      none
;~  algorithms          none
;~  data structures     none

;*  last modified       Christian Host-Madsen       2026.09.11      creation

;*  changed registers   none
;*  stack depth         0
gcd_stein_out:
    out     PORT_OUT                                                ; output low bit
    std     tmp_store                                               ; save low byte

    txa                                                             ; save high byte
    out     PORT_OUT
    ldd     tmp_store                                               ; reload low byte

    rts                                                             ; return
    nop                                                             ; [*] branch delay


.none
;-DATA-SEGMENT---------------------------------------------------------------------------------------------------------;
.data

tmp_store           db          ?                                   ; temporary storage
