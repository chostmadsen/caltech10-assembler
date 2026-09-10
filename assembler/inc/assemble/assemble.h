/**
 * inc/assemble/assemble.h
 * documentation @ src/assemble/assemble.c
 */

#pragma     once

#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {
            int         num_segs;
            ln_asm     *ln_asms;
} asm_ret;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] asm_ret assemble(const segmaps *segmap);                          // assembler assemble

void print_asm(const asm_ret *asm_r);                                           // print asm_ret struct
void print_asm_info(const asm_ret *asm_r);                                      // print asm_ret struct info

void free_asm_ret(asm_ret *asm_r);                                              // asm_ret free
