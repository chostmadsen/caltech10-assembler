/**
 * inc/assemble/line.h
 * documentation @ inc/assemble/line.c
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"
#include    "reader/reader.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

constexpr   size_t      LN_ASM_PAD          =   12;                             // ln_asm padding
typedef struct {                                                                // line assembly instruction (raw)
            int         instr;
            int         loc;
    const   src_f      *source;
            int         ln;
} ln_asm_;

constexpr   size_t      LN_ASM_ALN          =   nptwo_cap(sizeof(ln_asm_));     // ln_asm alignment
typedef struct {                                                                // line asssembly instruction (aligned)
    alignas(LN_ASM_ALN) ln_asm_     la;
} ln_asm;
_Static_assert((sizeof(ln_asm) & (sizeof(ln_asm) - 1)) == 0, "invalid ln_asm alignment");

constexpr   size_t      ASMS_PER_LN         =   (CACHE_LN_S <= sizeof(ln_asm)) ? 1 : CACHE_LN_S / sizeof(ln_asm);

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] ln_asm line_assemble( const segmaps *segmap,
                                    const ln_info *ln_inf  );                   // singular line assembly
