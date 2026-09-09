/**
 * src/assemble/assemble.c
 * Final assembler call.
 */

#include    "datastructures/stack.h"
#include    "reader/reader.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"
#include    "assemble/assemble.h"

void assemble(const segmaps *const segmap) {
    for (int i = 0; i < segmap->headmap.stmts.len; ++i) {
        const   ln_info *const  ln_inf  =   (ln_info*)peek_stack(&segmap->headmap.stmts, i);
        line_assemble(segmap, ln_inf);
    }
}
