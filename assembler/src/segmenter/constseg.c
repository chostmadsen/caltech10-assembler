/**
 * src/segmenter/constseg.c
 * .const evaluator.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <inttypes.h>

#include    "helpers/general.h"
#include    "output/external.h"
#include    "output/messages.h"
#include    "datastructures/hash.h"
#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "common/kwrds.h"
#include    "common/hash_tables/pseudo.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/constseg.h"

/*-CONSTSEG-OUTPUT----------------------------------------------------------------------------------------------------*/

/**
 * Constant var printer.
 *
 * @param       c_var           constant var
 */
static void print_const_var_(const const_var *const c_var) {                    // constant var printer
    printf("0x%016" PRIx64 " : ", c_var->head.hash);
    print_src_slice(&c_var->head.key, stdout);
    printf(" { v=%d }", c_var->val);
}

/**
 * Constant var stackmap printer.
 *
 * @param       smap            constant stackmap
 */
void print_const_map(const stackmap *const smap) {                              // constant stackmap printer
    // info
    printf( DEBUG_DELIM CLR_DIM " [[ constmap %zuitm(s)::%zubckts ]]\x1b[0m\n",
            smap->elements, smap->buckets                                       );

    // stackmap
    for (size_t i = 0; i < smap->buckets; ++i) {
        const   stack   *const  head_st =   smap->heads + i;
        printf("    [0x%x] - ", (int)i);
        for (int j = 0; j < head_st->len; ++j) {
            // variables
            print_const_var_((const_var*)peek_stack(head_st, j));
            if (j != head_st->len - 1)      printf(", ");
        }
        fputc('\n', stdout);
    }
    fputs(DEBUG_DELIM "\n", stdout);
}

/*-CONSTSEG-PARSER----------------------------------------------------------------------------------------------------*/

/**
 * Adds a constant variable to the existing map of constant variables, returning false if an error occurred adding the
 * value (either invalid number or existing item).
 *
 * @param       err_f           error report file
 * @param       sptr            string pointer
 * @param       smap            constant stackmap
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool const_chck_add_( rprt_f   *const err_f, 
                                           strptr   *const sptr, 
                                           stackmap *const smap   ) {           // constant stackmap add
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // get identifier
    const       size_t  slc_strt    =   sptr->col;
    src_slice           slc         =   { .str=sptr->str, .len=0 };
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);
    const_var           smap_itm    =   { .head=(smap_head){ .hash=hash_fnv1a_slc_lwr(&slc), .key=slc } };

    // check negative
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    const   bool    neg         =   *sptr->str == NEG_SYMB;
    if (neg)                        inc_strptr(sptr);

    // get value
    if (!('0' <= *sptr->str && *sptr->str <= '9') && *sptr->str != '$') {
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="expected number", .report_f=err_f },
                    "expected a number after `.const`"                                          );
        return  true;
    }

    // parse number
    const   int p_strt      =   sptr->col;
    const   int const_v     =   parse_num(err_f, sptr);
    if (const_v == -1)          return  true;
    const   int max_v       =   (neg) ? MAX_NUM_NEG : MAX_NUM;
    if (const_v > max_v) {
        err_f->col  =   p_strt;
        err_f->len  =   sptr->col - p_strt;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="number overflow", .report_f=err_f },
                    "maximum number range excession"                                            );
        return  true;
    }
    smap_itm.val                =   (neg) ? (const_v ^ MAX_NUM) + 1 : const_v;

    // check trailing characters
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str != CMMT_CHR && *sptr->str != '\0') {
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="trailing character(s)", .report_f=err_f },
                    "trailing character(s) after a `.const` definition"                               );
        return  true;
    }

    // check collisions
    const   smap_clsn_t clsn_t  =   stackmap_add_lwr(smap, &smap_itm);
    if (clsn_t == smap_lwr_clsn_t) {
        err_f->col  =   slc_strt;
        err_f->len  =   slc.len;
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="lowercase collision", .report_f=err_f },
                    "case-variant already defined"                                                   );
        return  true;
    } else if (clsn_t == smap_full_clsn_t) {
        err_f->col  =   slc_strt;
        err_f->len  =   slc.len;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="collision", .report_f=err_f },
                    "constant already defined"                                            );
        return  true;
    }
    return  false;
}

/**
 * Creates the lookup table of constants (only to be used in non-pseudo-op items).
 *
 * @param       source          source file
 * @param       err             error flag
 * @return                      constant stackmap
 */
[[nodiscard]] stackmap constseg(const src_f *const source, bool *const err) {   // constant stackmap creation
    // initialize constant table
    stackmap    smap    =   new_stackmap(CONST_BUCKETS, sizeof(const_var));

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr    =               { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDOOP_CHR)  continue;

        // check for .const
        inc_strptr(&sptr);
        size_t  n       =   0;
        for (; !is_whitespace(sptr.str[n]); ++n);
        if (pseudo_hash_lu(sptr.str, n).tok != tok_const)   continue;

        // add const value
        adj_strptr(&sptr, n);
        *err    =   const_chck_add_(&(rprt_f){ .file=source, .ln=i, .col=sptr.col }, &sptr, &smap);
    }

    // return populated constants
    return  smap;
}
