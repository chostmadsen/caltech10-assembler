/**
 * inc/assemble/line.h
 * documentation @ inc/assemble/line.c
 */

#pragma     once

#include    "reader/reader.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // line assembly instruction
            int         instr;
            int         ln;
} ln_asm;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] ln_asm line_assemble(const src_f *source, const segmaps *segmap, const ln_info *ln_inf); 
