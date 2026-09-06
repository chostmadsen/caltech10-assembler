/**
 * src/segmenter/dataseg.c
 * .data segment analyzer.
 */

#include    "helpers/general.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/dataseg.h"

[[nodiscard]] stackmap dataseg(const src_f *const source) {
    int     org     =   0;
    strptr  sptr    =   { .str=source->text, .idx=0 };
    for (size_t i = 0; i < source->ln_num; ++i) {
        if (source->text[source->ln_idxs[i]] != PSEUDOOP_CHR)   continue;
        sptr.idx    =   source->ln_idxs[i + 1];
        sptr.str    =   &source->text[sptr.idx];
    }
}

static void data_add(const src_f *const source, strptr *const sptr) {
    ;
}
