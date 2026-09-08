/**
 * src/segmenter/dataseg.c
 * .data segment analyzer.
 */

#include    <stddef.h>
#include    <stdio.h>

#include    "helpers/general.h"
#include    "common/kwrds.h"
#include    "common/hash_tables/pseudo.h"
#include    "common/gen_parse.h"
#include    "output/messages.h"
#include    "output/errors.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/dataseg.h"

/*-DATASEG-OUTPUT-----------------------------------------------------------------------------------------------------*/

/**
 * Data var printer.
 *
 * @param       d_var           data var
 */
static void print_data_var_(const void *const d_var_v) {                        // data var printer
    cit10a_asrt(d_var_v != nullptr);

    const   data_var    *const  d_var   =   (data_var*)d_var_v;
    print_var_tok(&d_var->var);
    printf(" { loc=0x%x }", d_var->loc);
}

/**
 * Data var stackmap printer.
 *
 * @param       smap            dat stackmap
 */
void print_data_map(const stackmap *const smap) {                               // data stackmap printer
    print_stackmap(smap, "datamap", print_data_var_);
}

/*-DATASEG-VERIFIERS--------------------------------------------------------------------------------------------------*/

/**
 * Verifies the .data start line (checks for trailing characters), then hands off if the next line exists.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool verify_data_strt_( strptr *const sptr,
                                             rprt_f *const err_f  ) {           // data start verification
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(err_f != nullptr);

    // check trailing characters
    if (check_ln_end(sptr, err_f))          return  true;

    if (newln_strptr(sptr, err_f->file))    sptr->str   =   nullptr;
    return  false;
}

/*-DATASEG-PARSER-----------------------------------------------------------------------------------------------------*/

/**
 * Adds a data variable to the existing map of data variables, returning false if an error occurred adding the
 * value (either invalid number or existing item). Sets the existing location
 *
 * @param       sptr            string pointer
 * @param       loc             data location
 * @param       smap            data stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool data_chck_add_( strptr   *const sptr,
                                          int             loc,
                                          stackmap *const smap,
                                          rprt_f   *const err_f ) {             // data add
    cit10a_asrt(sptr != nullptr && smap != nullptr);
    cit10a_asrt(loc >= 0);
    cit10a_asrt(err_f != nullptr);

    // get identifier (assume at first char)
    const       size_t  slc_strt    =   sptr->col;
    const   smap_head   s_head      =   get_identifier(sptr, err_f);
    if (s_head.key.str == nullptr)      return  true;
    data_var            smap_itm    =   { .var={ .head=s_head, .ln=sptr->ln + 1, .col=slc_strt }, .loc=loc };

    // data verify
    err_f->ln   =   sptr->ln;
    err_f->len  =   1;
    const   msg_info    store_err   =   { .type=msg_err_t, .header="storage error", .report_f=err_f };

    // verify storage types
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // indicator verify
    if (*sptr->str != STORAGE_IND) {
        // storage error
        err_f->col  =   sptr->col;
        cit10a_msg(&store_err, "invalid storage specifier");
        return  true;
    }

    // size verify
    inc_strptr(sptr);
    if (*sptr->str != STORAGE_BYTE) {
        // storage error
        err_f->col  =   sptr->col;
        cit10a_msg(&store_err, "invalid storage size specifier");
        return  true;
    }

    // verify initialization
    inc_strptr(sptr);
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str != STORAGE_INIT) {
        // init error
        err_f->col  =   sptr->col;
        cit10a_msg(&store_err, "no data initialization is permitted");
        return  true;
    }

    // check trailing characters
    inc_strptr(sptr);
    if (check_ln_end(sptr, err_f))  return  true;

    return  identifier_verify(smap, &smap_itm, err_f);
}

/**
 * Parses a .data segment until it ends, adjusting the .org as necessary.
 *
 * @param       sptr            string pointer
 * @param       org             data start location
 * @param       smap            data stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool dataseg_parse_( strptr   *const sptr,
                                          int      *const org,
                                          stackmap *const smap,
                                          rprt_f   *const err_f ) {             // data segment parse
    cit10a_asrt(sptr != nullptr && smap != nullptr);
    cit10a_asrt(org != nullptr && *org >= 0);
    cit10a_asrt(err_f != nullptr);

    // set flag
    bool    ret =   false;

    // segment section
    do {
        // get first character
        for (; is_whitespace(*sptr->str); inc_strptr(sptr));

        // check psuedo-ops
        if (*sptr->str == PSEUDO_STRT) {
            // check pseudo-op
            inc_strptr(sptr);
            size_t  n       =   0;
            for (; is_alphanum(sptr->str[n]); ++n);
            switch (pseudo_hash_lu(sptr->str, n).tok) {
                case tok_data:
                    // verify && skip
                    adj_strptr(sptr, n);
                    if      (verify_data_strt_(sptr, err_f))    ret =   true;
                    else if (sptr->str == nullptr)              return  ret;
                    break;
                case tok_code:
                    // break out of this (incremented in returned loop)
                    return  false;
                case tok_org:
                    // new org
                    adj_strptr(sptr, n);
                    const   int n_org   =   parse_org(sptr, err_f);
                    if (n_org == -1)        return  true;
                    *org                =   n_org;
                default:
                    // handled elsewhere (hopefully)
                    break;
            }
        } else if (*sptr->str != CMMT_CHR && *sptr->str != '\0') {
            // not line
            if (data_chck_add_(sptr, *org, smap, err_f))    ret =   true;
            ++(*org);
        }
    } while (!newln_strptr(sptr, err_f->file));
    return  ret;
}

/**
 * Creates the lookup table of data segment variables.
 *
 * @param       source          source file
 * @param       err             error flag
 * @return                      variable stackmap
 */
[[nodiscard]] stackmap dataseg(const src_f *const source, bool *const err) {    // data stackmap creation
    cit10a_asrt(err != nullptr && *err == false);
    cit10a_asrt(source != nullptr);

    // initialize data table
    stackmap    smap    =   new_stackmap(DATA_BUCKETS, sizeof(data_var));
    int         org     =   0;

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check for .data start
        inc_strptr(&sptr);
        size_t  n                   =   0;
        for (; is_alphanum(sptr.str[n]); ++n);
        if (pseudo_hash_lu(sptr.str, n).tok != tok_data)    continue;
        adj_strptr(&sptr, n);

        // check .data line
        rprt_f  err_f   =   (rprt_f){ .file=source, .ln=sptr.ln, .col=sptr.col};
        *err            =   verify_data_strt_(&sptr, &err_f);
        if (sptr.str == nullptr)    break;

        // parse .data
        *err            =   dataseg_parse_(&sptr, &org, &smap, &err_f);
        i               =   sptr.ln;
    }

    // return populated constants
    return  smap;
}
