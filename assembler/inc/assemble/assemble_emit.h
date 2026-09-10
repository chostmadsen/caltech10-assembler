/**
 * inc/assemble/assemble_emit.h
 * documentation @ src/assemble/assemble_emit.c
 */

#pragma     once

#include    "reader/reader.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/assemble.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

// & masks
constexpr   int         BYTE_S              =   8;
constexpr   int         LOWER_AND           =   (1 << BYTE_S) - 1;
constexpr   int         UPPER_AND           =   ((1 << (2 * BYTE_S)) - 1) ^ LOWER_AND;

// NOTE : these are adjusted for 16-bit, or 8-bit for data; adjust as needed
constexpr   int         B_PER_HEX           =   2;                              // number grouping out
constexpr   int         BIT_OUT             =   4;                              // number grouping out
_Static_assert(2 * B_PER_HEX == BIT_OUT, "output alignment requires this (adjust if this is changing)");

#define                 CODE_STRT               "  "                            // code definitions
#define                 SEG_SEP_ADRS                ""
#define                 CODE_SEP                      "  "
#define                 SEG_SEP_INSTR                     " "
#define                 CODE_END                             "    ;; "


#define                 DATA_STRT               ";          "                   // data definitions
#define                 DATA_END                             "    ;; "

#define                 ADRS_STRT               "; "                            // address definitions
#define                 ADRS_END                      "           ;; "

#define                 CMMT_STRT               ";;  "                          // commented out items

#define                 FULL_OUT                "                    "          // unaltered items

#define                 VERS_STRT               "; [*] "                        // version start

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void emit_asm( const char    *f_name, const src_f   *source,
               const segmaps *segmap, const asm_ret *asm_r   );                 // assembly emitter
