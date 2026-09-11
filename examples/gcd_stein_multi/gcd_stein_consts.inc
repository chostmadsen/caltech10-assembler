;-gcd_stein_consts.inc-------------------------------------------------------------------------------------------------;
;~  description         Caltech10 assembler GCD algorithm assembler testing (multi-file test).

;~  history             Christian Host-Madsen       2026.09.06      creation
;~                      Christian Host-Madsen       2026.09.11      multi-file split


;-CONSTANTS------------------------------------------------------------------------------------------------------------;

.const      LOW_BIT_MASK            0x01                            ; low bit check mask
.const      TST_LDI                 0x00                            ; ldi to make the equivalent of the `tst` instr
.const      PORT_OUT                0x05                            ; test output port
