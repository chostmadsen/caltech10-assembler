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
typedef struct {                                                                // line assembly instruction
            int         instr;
            int         loc;
    const   src_f      *source;
            int         ln;
            char        pad_[LN_ASM_PAD];
} ln_asm;
_Static_assert( (sizeof(ln_asm) & (sizeof(ln_asm) - 1)) == 0,
                "\n\n\nsizeof(ln_asm) must be a power of 2; change `LN_ASM_PAD`'s value "
                "(run tools/alignment_def.sh to calculate this value)\n\n"             );

constexpr   size_t      ASMS_PER_LN         =   (CACHE_LN_S <= sizeof(ln_asm)) ? 1 : CACHE_LN_S / sizeof(ln_asm);

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] ln_asm line_assemble( const segmaps *segmap,
                                    const ln_info *ln_inf  );                   // singular line assembly
