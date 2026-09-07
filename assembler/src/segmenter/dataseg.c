/**
 * src/segmenter/dataseg.c
 * .data segment analyzer.
 */

#include    "helpers/general.h"
#include    "common/hash_tables/pseudo.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/dataseg.h"

[[nodiscard]] static bool dataseg_itm_() {
    // something here
}

[[nodiscard]] static bool dataseg_parse_( rprt_f   *const err_f, 
                                          strptr   *const sptr, 
                                          stackmap *const smap,
                                          int      *const org    ) {            // data segment parse
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str == PSEUDOOP_CHR) {
        // check breakout
        inc_strptr(sptr);
        size_t  n       =   0;
        for (; !is_whitespace(sptr->str[n]); ++n);
        // switch (pseudo_hash_lu(sptr->str, n).tok) { }
    }
}

/**
 * Creates the lookup table of data segment variables.
 *
 * @param       source          source file
 * @param       err             error flag
 * @return                      variable stackmap
 */
[[nodiscard]] stackmap dataseg(const src_f *const source, bool *const err) {    // data stackmap creation
    // initialize data table
    stackmap    smap    =   new_stackmap(DATA_BUCKETS, sizeof(data_var));
    int         org     =   0;

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr    =               { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDOOP_CHR)  continue;

        // check for .data start
        inc_strptr(&sptr);
        size_t  n       =   0;
        for (; !is_whitespace(sptr.str[n]); ++n);
        // if (pseudo_hash_lu(sptr.str, n).tok != tok_data)    continue;
        // TODO : adjust over 1

        // parse .data segment
        adj_strptr(&sptr, n);
        *err    =   dataseg_parse_(&(rprt_f){ .file=source, .col=sptr.col }, &sptr, &smap, &org);
        i       =   sptr.ln;
    }

    // return populated constants
    return  smap;
}
