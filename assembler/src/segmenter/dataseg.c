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
        for (; !is_whitespace(sptr->str[n]) && sptr->str[n] != '\0'; ++n);
        switch (pseudo_hash_lu(sptr->str, n).tok) {
            case tok_data:  /* verify, but do nothing */;
            case tok_code:  /* break out of this */;
            case tok_org:   /* modifiy org */;
            default:        /* skip line - handled elsewhere (hopefully) */;
        }
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
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDOOP_CHR)  continue;

        // check for .data start
        inc_strptr(&sptr);
        size_t  n                   =   0;
        for (; !is_whitespace(sptr.str[n]) && sptr.str[n] != '\0'; ++n);
        if (pseudo_hash_lu(sptr.str, n).tok != tok_data)    continue;

        // check trailing characters
        adj_strptr(&sptr, n);
        rprt_f  err_f               =   { .file=source, .col=sptr.col };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != CMMT_CHR && *sptr.str != '\0') {
            err_f.col   =   sptr.col;
            err_f.ln    =   i;
            err_f.len   =   1;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="trailing character", .report_f=&err_f },
                        "trailing character after `.data` start"                                        );
            *err        =   true;
        }

        // parse .data segment
        if (i + 1 >= source->ln_num)    break;
        sptr.str    =   src_f_getline(source, i + 1);
        sptr.ln     =   i + 1;
        sptr.col    =   0;
        *err        =   dataseg_parse_(&(rprt_f){ .file=source, .col=sptr.col }, &sptr, &smap, &org);
        i           =   sptr.ln;
    }

    // return populated constants
    return  smap;
}
