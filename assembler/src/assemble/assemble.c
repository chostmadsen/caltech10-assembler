/**
 * src/assemble/assemble.c
 * Final assembler call.
 */

#include    <pthread.h>
#include    <stdatomic.h>

#include    "datastructures/stack.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"
#include    "assemble/assemble.h"

/*-STORAGE-ITEMS------------------------------------------------------------------------------------------------------*/

static      atomic_bool     asm_end_flg;                                        // assembler end assembly flag

/*-ASSEMBLER-ORCHESTRATOR---------------------------------------------------------------------------------------------*/

[[nodiscard]] asm_ret assemble(const segmaps *const segmap) {
    atomic_store_explicit(&asm_end_flg, false, memory_order_relaxed);

    for (int i = 0; i < segmap->headmap.stmts.len; ++i) {
        const   ln_info *const  ln_inf  =   (ln_info*)peek_stack(&segmap->headmap.stmts, i);
        line_assemble(segmap, ln_inf);
    }
}
