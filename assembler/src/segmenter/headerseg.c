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
                                                 int           loc,
                                                 rprt_f *const err_f ) {        // code range check
    --loc;
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
 * @param       hmap_v          header stackmap
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool header_chck_add_( strptr *const sptr,
                                            int    *const loc,
                                            void   *const hmap_v,
                                            rprt_f *const err_f   ) {           // header add
    cit10a_asrt(sptr != nullptr && hmap_v != nullptr);
    cit10a_asrt(*loc >= 0);
    cit10a_asrt(err_f != nullptr);

    bool    ret     =   false;

    // get identifier (assume at first char)
    const       size_t  slc_strt    =   sptr->col;
    const   smap_head   s_head      =   get_identifier(sptr, err_f);
    if (s_head.key.str == nullptr)      return  true;
    for(; is_whitespace(*sptr->str); inc_strptr(sptr));

    if (*sptr->str == HEADER_CHR) {
        // add header
        header_var          smap_itm    =   { .var={ .head=s_head,     .source=err_f->file,
                                                     .ln=sptr->ln + 1, .col=slc_strt        },
                                              .loc=*loc                                        };
        ret                             =   identifier_verify(&((headermap*)(hmap_v))->smap, &smap_itm, err_f);

        // check for additional code
        for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
        if (*sptr->str == CMMT_CHR || *sptr->str == '\0')   return  ret;
    }

    const   ln_info     ln_inf                  =   { .loc=(*loc)++, .source=err_f->file, .ln=sptr->ln };
    if (!code_rnge_chck_(sptr->ln, *loc, err_f))    code_chck_dup_(&ln_inf, &((headermap*)hmap_v)->stmts, err_f);
    push_stack(&((headermap*)hmap_v)->stmts, &ln_inf);
    return  ret;
}

/**
 * Creates the lookup table of headers, and finds the total number of statements.
 *
 * @param       org             code start location
 * @param       source          source file
 * @param       srcs            sources
 * @param       err             error flag
 * @param       hmap            headermap
 * @return                      whether an error occurred
 */
[[nodiscard]] bool headerseg(       int       *const org,
                              const src_f     *const source,
                              const sources   *const srcs,
                                    headermap *const hmap    ) {                // header stackmap creation
    return  commonseg(org, source, srcs, tok_code, header_chck_add_, hmap);
}
