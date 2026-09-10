/**
 * src/segmenter/headerseg.c
 * .code header segment analyzer.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <stdint.h>
#include    <string.h>

#include    "helpers/general.h"
#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "argparse/argparse.h"
#include    "common/kwrds.h"
#include    "common/gen_parse.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"
#include    "segmenter/headerseg.h"

/*-CODESEG-BIT-CHECKERS-----------------------------------------------------------------------------------------------*/

alignas(CACHE_LN_S) static  uint64_t    c_locs[N_H_FIELDS]  =   { 0 };          // field bitfield

/*-HEADER-OUTPUT------------------------------------------------------------------------------------------------------*/

/**
 * Header printer.
 *
 * @param       h_var           header
 */
static void print_header_var_(const void *const h_var_v) {                      // header printer
    cit10a_asrt(h_var_v != nullptr);

    const   header_var  *const  h_var   =   (header_var*)h_var_v;
    print_var_tok(&h_var->var);
    printf(" { loc=0x%04x }", h_var->loc);
}

/**
 * Header var stackmap printer.
 *
 * @param       smap            header stackmap
 */
void print_header_map(const headermap *const hmap) {                            // header stackmap printer
    cit10a_asrt(hmap != nullptr);

    print_stackmap(&hmap->smap, print_header_var_);
    printf(CLR_DIM "    ( %d stmts )\x1b[0m\n", hmap->stmts.len);
}

/*-CODESEG-PARSER-(HEADERS-ONLY)--------------------------------------------------------------------------------------*/

/**
 * Check for overlapping code.
 *
 * @param       ln_inf          line info
 * @param       st              code stack
 * @param       err_f           error report file
 */
static void code_chck_dup_( const ln_info *const ln_inf,
                            const stack   *const st,
                                  rprt_f  *const err_f   ) {                    // code overlap checker
    // check for no duplicate or supression
    if (c_args.warnings.noverc)                         return;
    if (!set_bitmap(c_locs, N_H_FIELDS, ln_inf->loc))   return;

    // setup error
    bool    found_overlap       =   false;
    const   char    *const  ln  =   src_f_getline(ln_inf->source, ln_inf->ln);
    int                     idx =   0;
    for (; is_whitespace(ln[idx]); ++idx);
    err_f->ln       =   ln_inf->ln;
    err_f->len      =   1;
    err_f->col      =   idx;

    for (int i = 0; i < st->len; ++i) {
        const   ln_info     st_inf  =   *(ln_info*)peek_stack(st, i);
        if (st_inf.loc != ln_inf->loc)  continue;
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="overlapping code", .report_f=err_f },
                    "overlap with code declared at 0x%04x [ @ %s::%d ]",
                    ln_inf->loc, st_inf.source->f_name, st_inf.ln + 1                             );
        found_overlap   =   true;
    }

    if (found_overlap)      return;
    cit10a_msg( &(msg_info){ .type=msg_intrnl_wrn_t, .header="overlap miss"}, 
                "an overlapping data item is declared at %s::%d::%d, but it couldn't be found",
                err_f->file->f_name, err_f->ln, err_f->col                                      );
}
/**
 * Check the range the given code item.
 *
 * @param       ln              current line
 * @param       err_f           error report file
 */
[[nodiscard]] static bool code_rnge_chck_( const int           ln,
                                           const int           loc,
                                                 rprt_f *const err_f ) {        // code range check
    if (loc <= (int)MAX_ADRS)       return  false;

    // setup error information
    strptr              sptr    =   { .str=src_f_getline(err_f->file, ln), .col=0 };
    for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
    src_slice           slc     =   { .str=sptr.str, .len=0 };
    for (; is_alphanum(*sptr.str); inc_strptr(&sptr), ++slc.len);
    const   var_tok     ln_inf  =   { .ln=ln + 1, .col=sptr.col - slc.len, .head.key=slc };
    range_msg(&ln_inf, err_f, loc, MAX_ADRS, msg_warn_t);
    return  true;
}

/**
 * Adds a header to the existing map of headers, returning false if an error occurred adding the value (either invalid
 * number or existing item). Sets the existing location.
 *
 * @param       sptr            string pointer
 * @param       loc             code location
 * @param       smap            header stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool header_chck_add_( strptr    *const sptr,
                                            int       *const loc,
                                            headermap *const hmap,
                                            rprt_f    *const err_f  ) {         // header add
    cit10a_asrt(sptr != nullptr && hmap != nullptr);
    cit10a_asrt(*loc >= 0);
    cit10a_asrt(err_f != nullptr);

    bool    ret     =   false;

    // get identifier (assume at first char)
    const       size_t  slc_strt    =   sptr->col;
    const   smap_head   s_head      =   get_identifier(sptr, err_f);
    if (s_head.key.str == nullptr)      return  true;
    for(; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str != HEADER_CHR) {
        // code line - skip
        goto    add_code;
    }

    // add header
    header_var          smap_itm    =   { .var={ .head=s_head,     .source=err_f->file,
                                                 .ln=sptr->ln + 1, .col=slc_strt        },
                                          .loc=*loc                                        };
    ret                             =   identifier_verify(&hmap->smap, &smap_itm, err_f);

    // check for additional code
    for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str == CMMT_CHR || *sptr->str == '\0')   return  ret;

add_code:
    const   ln_info     ln_inf                  =   { .loc=(*loc)++, .source=err_f->file, .ln=sptr->ln };
    if (!code_rnge_chck_(sptr->ln, *loc, err_f))    code_chck_dup_(&ln_inf, &hmap->stmts, err_f);
    push_stack(&hmap->stmts, &ln_inf);
    return  ret;
}

/**
 * Parses a .code segment until it ends for headers, adjusting the .org as necessary.
 *
 * @param       sptr            string pointer
 * @param       org             code start location
 * @param       smap            header stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool codeseg_parse_( strptr    *const sptr,
                                          int       *const org,
                                          headermap *const hmap,
                                          rprt_f    *const err_f  ) {           // code segment parse
    cit10a_asrt(sptr != nullptr && hmap != nullptr);
    cit10a_asrt(org != nullptr && *org >= 0);
    cit10a_asrt(err_f != nullptr);

    // set flag
    bool    ret =   false;

    // segment section
    do {
        // get first character
        for (; is_whitespace(*sptr->str); inc_strptr(sptr));
        err_f->ln   =   sptr->ln;

        // check psuedo-ops
        if (*sptr->str == PSEUDO_STRT) {
            // check pseudo-op
            switch (pseudo_hash_lu_adj(sptr)) {
                case tok_code:
                    // verify && skip
                    if      (verify_sctn_strt(sptr, err_f))         ret =   true;
                    else if (sptr->str == nullptr)                  return  ret;
                    break;
                case tok_none:  [[fallthrough]];
                case tok_data:
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
            if (header_chck_add_(sptr, org, hmap, err_f))    ret =   true;
        }
    } while (!newln_strptr(sptr, err_f->file));

    return  ret;
}

/**
 * Creates the lookup table of headers, and finds the total number of statements.
 *
 * @param       source          source file
 * @param       err             error flag
 * @return                      header stackmap
 */
[[nodiscard]] bool headerseg(const src_f *const source, headermap *const hmap) {// header stackmap creation
    cit10a_asrt(source != nullptr);
    cit10a_asrt(hmap != nullptr);

    // initialize header items
    int         org     =   0;
    bool        err     =   false;

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check for .code start
        if (pseudo_hash_lu_adj(&sptr) != tok_code)          continue;

        // check .code line
        rprt_f  err_f   =   (rprt_f){ .file=source, .ln=sptr.ln, .col=sptr.col};
        if (verify_sctn_strt(&sptr, &err_f))                err =   true;
        if (sptr.str == nullptr)                            break;

        // parse .code
        if (codeseg_parse_(&sptr, &org, hmap, &err_f))      err =   true;
        i               =   sptr.ln;
    }

    // return populated constants
    return  err;
}
