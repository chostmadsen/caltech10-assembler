/**
 * src/segmenter/dataseg.c
 * .data segment analyzer.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <stdint.h>
#include    <string.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "common/kwrds.h"
#include    "common/gen_parse.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/dataseg.h"

/*-DATASEG-BIT-CHECKERS-----------------------------------------------------------------------------------------------*/

alignas(CACHE_LN_S) static  uint64_t    d_locs[N_D_FIELDS]  =   { 0 };          // field bitfield

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
 * @param       smap            data stackmap
 */
void print_data_map(const stackmap *const smap) {                               // data stackmap printer
    cit10a_asrt(smap != nullptr);

    print_stackmap(smap, print_data_var_);
}

/*-DATASEG-PARSER-----------------------------------------------------------------------------------------------------*/

/**
 * Check the range the given data item. Critical error, as instructions no longer work.
 *
 * @param       d_var           data var
 * @param       err_f           error report file
 * @return                      whether the data is out of range
 */
[[nodiscard]] static bool data_rnge_chck_( const data_var *const d_var,
                                                 rprt_f   *const err_f  ) {     // data range check
    if (d_var->loc <= (int)MAX_NUM)         return  false;

    // out of range
    range_msg(&d_var->var, err_f, d_var->loc, MAX_NUM, msg_err_t);
    return  true;
}

/**
 * Check for data reserved identifiers.
 *
 * @param       d_var           data var
 * @param       err_f           error report file
 * @return                      whether the identifier is reserved
 */
[[nodiscard]] static bool data_chck_rsrv( const data_var *const d_var,
                                                rprt_f   *const err_f) {        // reserved identifier check
    if (d_var->var.head.key.len != 1)       return  false;

    // check identifier match
    for (size_t i = 0; i < arr_s(RSRVD_IDENT); ++i) {
        if (to_lwr_chr(*d_var->var.head.key.str) != RSRVD_IDENT[i])     continue;

        // report error
        err_f->len      =   d_var->var.head.key.len;
        err_f->col      =   d_var->var.col;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="reserved identifier", .report_f=err_f },
                    "identifier `%c` is reserved", *d_var->var.head.key.str                         );
        return  true;
    }
    return  false;
}

/**
 * Check for overlapping data definitions.
 *
 * @param       d_var           data var
 * @param       smap            data stackmap
 * @param       err_f           error report file
 */
static void data_chck_dup_( const data_var *const d_var,
                            const stackmap *const smap,
                                  rprt_f   *const err_f  ) {                    // data overlap checker
    // check for no duplicate or supression
    if (c_args.warnings.noverd)                         return;
    if (!set_bitmap(d_locs, N_D_FIELDS, d_var->loc))    return;
    bool    found_overlap   =   false;

    // setup error
    err_f->len      =   d_var->var.head.key.len;
    err_f->col      =   d_var->var.col;
    for (size_t i = 0; i < smap->buckets; ++i) {
        // check stackmap
        for (int j = 0; j < smap->heads[i].len; ++j) {
            // check stack
            const   data_var    s_var   =   *(data_var*)peek_stack(&smap->heads[i], j);
            if (s_var.loc != d_var->loc)    continue;
            cit10a_msg(&(msg_info){ .type=msg_warn_t, .header="overlapping data", .report_f=err_f},
                        "overlap with data declared at 0x%02x [ @ %s::%d::%d ]",
                        d_var->loc, s_var.var.source->f_name, s_var.var.ln, s_var.var.col           );
            found_overlap   =   true;
        }
    }

    if (found_overlap)      return;
    cit10a_msg( &(msg_info){ .type=msg_intrnl_wrn_t, .header="overlap miss"}, 
                "an overlapping data item is declared at %s::%d::%d, but it couldn't be found",
                err_f->file->f_name, err_f->ln, err_f->col                                      );
}

/**
 * Adds a data variable to the existing map of data variables, returning false if an error occurred adding the value
 * (either invalid number or existing item). Sets the existing location.
 *
 * @param       sptr            string pointer
 * @param       loc             data location
 * @param       smap            data stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool data_chck_add_( strptr   *const sptr,
                                          int      *const loc,
                                          stackmap *const smap,
                                          rprt_f   *const err_f ) {             // data add
    cit10a_asrt(sptr != nullptr && smap != nullptr);
    cit10a_asrt(*loc >= 0);
    cit10a_asrt(err_f != nullptr);

    // get identifier (assume at first char)
    const   size_t      slc_strt    =   sptr->col;
    const   smap_head   s_head      =   get_identifier(sptr, err_f);
    if (s_head.key.str == nullptr)      return  true;
    data_var            smap_itm    =   { .var={ .head=s_head,     .source=err_f->file,
                                                 .ln=sptr->ln + 1, .col=slc_strt        },
                                          .loc=*loc                                        };

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
    if (check_ln_end(sptr, err_f))              return  true;
    if (data_chck_rsrv(&smap_itm, err_f))       return  true;

    if (data_rnge_chck_(&smap_itm, err_f))      return  true;
    data_chck_dup_(&smap_itm, smap, err_f);
    const   bool    ret =   identifier_verify(smap, &smap_itm, err_f);
    ++(*loc);
    return  ret;
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
        err_f->ln   =   sptr->ln;
        for (; is_whitespace(*sptr->str); inc_strptr(sptr));

        // check psuedo-ops
        if (*sptr->str == PSEUDO_STRT) {
            // check pseudo-op
            switch (pseudo_hash_lu_adj(sptr)) {
                case tok_data:
                    // verify && skip
                    if      (verify_sctn_strt(sptr, err_f))     ret =   true;
                    else if (sptr->str == nullptr)              return  ret;
                    break;
                case tok_none:  [[fallthrough]];
                case tok_code:
                    // break out of this (incremented in returned loop)
                    return  false;
                case tok_org:
                    // new org
                    const   int n_org   =   parse_org(sptr, err_f);
                    if (n_org == -1)        return  true;
                    *org                =   n_org;
                    break;
                default:
                    // handled elsewhere (hopefully)
                    break;
            }
        } else if (*sptr->str != CMMT_CHR && *sptr->str != '\0') {
            // skip blank lines
            if (data_chck_add_(sptr, org, smap, err_f))         ret =   true;
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
[[nodiscard]] bool dataseg(const src_f *const source, stackmap *const smap) {   // data stackmap creation
    cit10a_asrt(source != nullptr);
    cit10a_asrt(smap != nullptr);

    // initialize data items
    bool        err     =   false;
    int         org     =   0;

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check for .data start
        if (pseudo_hash_lu_adj(&sptr) != tok_data)          continue;

        // check .data line
        rprt_f  err_f   =   (rprt_f){ .file=source, .ln=sptr.ln, .col=sptr.col};
        if (verify_sctn_strt(&sptr, &err_f))                err =   true;
        if (sptr.str == nullptr)    break;

        // parse .data
        if (dataseg_parse_(&sptr, &org, smap, &err_f))      err =   true;
        i               =   sptr.ln;
    }

    // return populated constants
    return  err;
}
