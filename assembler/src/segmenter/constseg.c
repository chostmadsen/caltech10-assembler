/**
 * src/segmenter/constseg.c
 * .const evaluator.
 */

#include    <stddef.h>
#include    <stdio.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "datastructures/stackmap.h"
#include    "common/kwrds.h"
#include    "common/gen_parse.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/constseg.h"

/*-CONSTSEG-OUTPUT----------------------------------------------------------------------------------------------------*/

/**
 * Constant var printer.
 *
 * @param       c_var           constant var
 */
static void print_const_var_(const void *const c_var_v) {                       // constant var printer
    cit10a_asrt(c_var_v != nullptr);

    const   const_var   *const  c_var   =   (const_var*)c_var_v;
    print_var_tok(&c_var->var);
    printf(" { v=0x%x }", c_var->val);
}

/**
 * Constant var stackmap printer.
 *
 * @param       smap            constant stackmap
 */
void print_const_map(const stackmap *const smap) {                              // constant stackmap printer
    print_stackmap(smap, "constmap", print_const_var_);
}

/*-CONSTSEG-PARSER----------------------------------------------------------------------------------------------------*/

/**
 * Adds a constant variable to the existing map of constant variables, returning false if an error occurred adding the
 * value (either invalid number or existing item).
 *
 * @param       sptr            string pointer
 * @param       smap            constant stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool const_chck_add_( strptr   *const sptr,
                                           stackmap *const smap,
                                           rprt_f   *const err_f ) {            // constant stackmap add
    cit10a_asrt(sptr != nullptr && smap != nullptr);
    cit10a_asrt(err_f != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // get identifier
    const   size_t      slc_strt    =   sptr->col;
    const   smap_head   s_head      =   get_identifier(sptr, err_f);
    if (s_head.key.str == nullptr)      return  true;
    const_var           smap_itm    =   { .var={ .head=s_head, .ln=sptr->ln + 1, .col=slc_strt } };

    // get number value
    const   int         ret         =   parse_num_repr(sptr, err_f);
    if (ret == -1)                      return  true;
    smap_itm.val                    =   ret;

    // check trailing characters
    if (check_ln_end(sptr, err_f))  return  true;

    return  identifier_verify(smap, &smap_itm, err_f);
}

/**
 * Creates the lookup table of constants (only to be used in non-pseudo-op items).
 *
 * @param       source          source file
 * @param       err             error flag
 * @return                      constant stackmap
 */
[[nodiscard]] stackmap constseg(const src_f *const source, bool *const err) {   // constant stackmap creation
    cit10a_asrt(err != nullptr && *err == false);
    cit10a_asrt(source != nullptr);

    // initialize constant table
    stackmap    smap    =   new_stackmap(CONST_BUCKETS, sizeof(const_var));

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check for .const
        if (pseudo_hash_lu_adj(&sptr) != tok_const)         continue;

        // add const value
        *err    =   const_chck_add_(&sptr, &smap, &(rprt_f){ .file=source, .ln=i, .col=sptr.col });
    }

    // return populated constants
    return  smap;
}
