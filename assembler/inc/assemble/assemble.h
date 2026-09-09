/**
 * inc/assemble/assemble.h
 * documentation @ src/assemble/assemble.c
 */

#pragma     once

#include    "helpers/general.h"
#include    "datastructures/stack.h"
#include    "segmenter/seg_orch.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {
} aln_stack;

typedef struct {
            int         num_segs;
} asm_ret;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] asm_ret assemble(const segmaps *segmap); 
