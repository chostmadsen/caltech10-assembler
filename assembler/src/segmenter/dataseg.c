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
    for (size_t i = 0; i < source->ln_num; ++i) {
        strptr  sptr    =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str == PSEUDOOP_CHR)  printf("%s - here\n", sptr.str);
    }
}

static void data_add(const src_f *const source, strptr *const sptr) {
    ;
}
