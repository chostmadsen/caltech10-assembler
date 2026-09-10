/**
 * tools/alignment_def.c
 * Alignment tools for building the assembler.
 */

#include    <stddef.h>
#include    <stdio.h>

/*-ALIGNMENT_DEF-ITEMS------------------------------------------------------------------------------------------------*/

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE                         128
#endif  /* CACHE_LINE_SIZE */
constexpr   unsigned    CACHE_LN_S_ALN      =   CACHE_LINE_SIZE;                // cache line size
_Static_assert(CACHE_LN_S_ALN > 0, "cache line size must be a positive integer");
_Static_assert(!(CACHE_LN_S_ALN & (CACHE_LN_S_ALN - 1)), "cache line size must be a power of two");

constexpr   int         OUT_TAB_ALN         =   31;

// NOTE : Keep the following synced with `ln_asm` with `pad_` omitted.
typedef struct {                                                                // line assembly instruction (non-pad)
            int         instr;
            int         loc;
    const   void       *source;
            int         ln;
} ln_asm_loc;

/*-ALIGNMENT_DEF-MAIN-------------------------------------------------------------------------------------------------*/

int main(void) {
    // struct dump
    printf( "\x1b[2mstruct `ln_asm` dump "
            "(ensure this matches `ln_asm` in assembler/inc/assemble/line.h, without the field pad_)\n\n" );
    printf("instr:      %3zu     size:   %3zu\n", offsetof(ln_asm_loc, instr),  sizeof(int));
    printf("loc:        %3zu     size:   %3zu\n", offsetof(ln_asm_loc, loc),    sizeof(int));
    printf("source:     %3zu     size:   %3zu\n", offsetof(ln_asm_loc, source), sizeof(void*));
    printf("ln:         %3zu     size:   %3zu\n", offsetof(ln_asm_loc, ln),     sizeof(int));
    printf( "                    sum:    %3zu\n", sizeof(ln_asm_loc));

    // calculate padding size
    const   size_t  used    =   offsetof(ln_asm_loc, ln) + sizeof(int);
    size_t          targ    =   1;
    for (; targ < sizeof(ln_asm_loc); targ <<= 1);
    const   size_t  ln_asm_aln      =   targ - used;

    // padding size output
    if (ln_asm_aln != 0) {
        printf("\nIn assembler/assemble/line.h, change `LN_ASM_PAD` to the following.\n");
        printf("\x1b[0m");
        printf( "constexpr   size_t      LN_ASM_PAD          =   %zu;%*s"
                "// ln_asm padding\n\n", ln_asm_aln, OUT_TAB_ALN - snprintf(nullptr, 0, "%zu", ln_asm_aln), "" );
    } else {
        printf("\n\x1b[0mIn assembler/inc/assemble/line.h, remove the `pad_` field from `ln_asm` entirely.\n");
    }
    return  0;
}
