/*
 * src/segmenter/segment.c
 * Segmenter common items.
 */

#include    <stddef.h>
#include    <string.h>
#include    <stdint.h>
#include    <inttypes.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/argparse.h"
#include    "datastructures/hash.h"
#include    "datastructures/stackmap.h"
#include    "common/kwrds.h"
#include    "common/gen_parse.h"
#include    "perfhash/inc/hash_table.h"
#include    "segmenter/segment.h"

/*-PARSER-HELPERS-----------------------------------------------------------------------------------------------------*/

/**
 * Gets the alphanumeric identifier (first character must be alphanumeric). Consumes it with the given strptr.
 * Sets up an smap head.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      stackmap head item
 */
[[nodiscard]] smap_head get_identifier( strptr *const sptr,
                                        rprt_f *const err_f ) {                 // identifier getter
    cit10a_asrt(sptr != nullptr);

    src_slice       slc     =   get_ident(sptr, err_f);
    if (slc.len == 0) {
        // invalid identifier
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid identifier", .report_f=err_f },
                    "invalid identifier"                                                           );
        return  (smap_head){ .hash=0, .key.str=nullptr, .key.len=0 };
    }

    // get and return identifier
    return  (smap_head){ .hash=hash_fnv1a_slc_lwr(&slc), .key=slc };
}

/*-BITMAP-SETTERS-/-OVERLAP-CHECKERS----------------------------------------------------------------------------------*/

/**
 * Sets a bitset, with the given number of fields. Always sets.
 *
 * @param       bmap            bitmap (bitset)
 * @param       n_flds          number of fields
 * @param       fld             field
 * @return                      whether the field was set
 */
[[nodiscard]] bool set_bitmap(       uint64_t *const bmap,
              [[maybe_unused]] const int             n_flds,
                               const int             fld     ) {                // bitset set
    cit10a_asrt(bmap != nullptr);
    cit10a_asrt(n_flds > 0);

    // find mask and set
    const   int         shft_idx    =   fld >> FLDS_PER;
    cit10a_asrt(shft_idx < n_flds);
    const   uint64_t    shft        =   (uint64_t)1 << (fld & (FLD_SHFT - 1));
    const   bool        is_set      =   bmap[shft_idx] & shft;
    bmap[shft_idx]                  |=  shft;
    return  is_set;
}

/*-.PSEUDO-OP-PARSERS-------------------------------------------------------------------------------------------------*/

/**
 * Try to return item after a .org specification.
 *
 * @param       sptr            sptr
 * @param       err_f           error report file
 * @return                      number
 */
[[nodiscard]] int parse_org(strptr *const sptr, rprt_f *const err_f) {          // .org parser
    cit10a_asrt(sptr != nullptr);

    // find consptr
    while (is_whitespace(*sptr->str))   inc_strptr(sptr);

    // try to parse number
    if (('0' <= *sptr->str && *sptr->str <= '9') || *sptr->str == HEX_CHR_ALT) {
        const   int     ret         =   parse_num_adrs(sptr, err_f);
        if (ret == -1)                  return  ret;

        // verify trailing items
        if (check_ln_end(sptr, err_f))  return  -1;
        return  ret;
    }

    // no number
    err_f->ln   =   sptr->ln;
    err_f->col  =   sptr->col;
    err_f->len  =   1;
    cit10a_msg( &(msg_info){ .type=msg_err_t, .header="expected number", .report_f=err_f},
                "expected a number after `.org`"                                           );
    return  -1;
}

/**
 * Verifies a .section start line (checks for trailing characters), then hands off if the next line exists.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] bool verify_sctn_strt(strptr *const sptr, rprt_f *const err_f) {  // .section start verification
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(err_f != nullptr);

    // check trailing characters
    if (check_ln_end(sptr, err_f))          return  true;

    if (newln_strptr(sptr, err_f->file))    sptr->str   =   nullptr;
    return  false;
}

/*-RANGE-CHECKERS-----------------------------------------------------------------------------------------------------*/

/**
 * Out of range warning message.
 *
 * @param       var             variable
 * @param       err_f           error report file
 * @param       loc             current location
 * @param       max             maximum range.
 * @param       msg_t           message type
 */
void range_msg( const var_tok *const var,
                      rprt_f  *const err_f,
                const int            loc,
                const int            max,
                const msg_info_t     msg_t  ) {                                  // range check error message
    cit10a_asrt(var != nullptr);
    cit10a_asrt(err_f != nullptr);

    // invalid data range
    char    key[var->head.key.len + 1];
    memcpy(key, var->head.key.str, var->head.key.len);
    key[var->head.key.len]    =   '\0';

    // arg setup
    const   size_t              ln      =   var->ln;
    const   size_t              col     =   var->col + 1;
    err_f->ln   =   ln - 1;
    err_f->col  =   col - 1;
    err_f->len  =   var->head.key.len;
    cit10a_msg( &(msg_info){ .type=msg_t, .header="oob data", .report_f=err_f },
                "Outside of the maximum range 0x%x (now at 0x%x)", max, loc);
}

/*-STACKMAP-REPEAT-OUTPUT-VERIFIER------------------------------------------------------------------------------------*/

/**
 * Verify identifier pushed to stackmap. `smap_itm` must have the header `var_tok` to have proper error diagnostics.
 *
 * @param       smap            stackmap to push to
 * @param       smap_itm        stackmap item
 * @param       err_f           error report file
 * @return                      whether a critical error occurred pushing an identifier
 */
[[nodiscard]] bool identifier_verify(       stackmap *const smap,
                                      const void     *const smap_itm, 
                                            rprt_f   *const err_f     ) {       // identifier verification
    cit10a_asrt(smap != nullptr);
    cit10a_asrt(smap_itm != nullptr);
    cit10a_asrt(err_f != nullptr);

    // check collisions
    const   smap_clsn_t         clsn_t  =   stackmap_add_lwr(smap, smap_itm);
    const   var_tok     *const  head    =   (var_tok*)smap_itm;
    if (clsn_t == smap_no_clsn_t)           return  false;

    // set locations
    err_f->col  =   head->col;
    err_f->len  =   head->head.key.len;

    // get redefintion
    const   var_tok     *const  redef   =   (clsn_t == smap_lwr_clsn_t) ? 
                                            stackmap_get_h_lwr(smap, &head->head.key, head->head.hash) :
                                            stackmap_get_h(smap, &head->head.key, head->head.hash);
    cit10a_asrt(redef != nullptr);
    const   size_t              ln      =   redef->ln;
    const   size_t              col     =   redef->col + 1;
    const   size_t              col_e   =   redef->col + redef->head.key.len;
    const   char    *const      file    =   redef->source->f_name;

    if (clsn_t == smap_lwr_clsn_t && !c_args.case_sens) {
        // get old def
        char                    key[head->head.key.len + 1];
        memcpy(key, redef->head.key.str, head->head.key.len);
        key[head->head.key.len]         =   '\0';

        // case collision
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="case-variant redefinition", .report_f=err_f },
                    "case-variant redefinition of identifier [ %s @ %s::%d::%d-%d ]", key, file, ln, col, col_e );
        return  true;
    } else if (clsn_t == smap_full_clsn_t){
        // full collision
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="redefinition", .report_f=err_f },
                "redefinition of identifier [ @ %s::%d::%d-%d ]", file, ln, col, col_e       );
        return  true;
    }
    return  false;
}

/*-SECTION-ITERATORS--------------------------------------------------------------------------------------------------*/

/**
 * Parses a section segment until it ends, adjusting the .org as necessary.
 *
 * @param       sptr            string pointer
 * @param       org             start location
 * @param       map             headermap or stackmap
 * @param       srcs            sources
 * @param       tok             match token
 * @param       fn              inner add function call
 * @param       err_f           error report file
 * @return                      whether an error occurred
 */
[[nodiscard]] static bool commonseg_parse_(       strptr     *const sptr,
                                                  int        *const org,
                                                  void       *const map,
                                            const sources    *const srcs,
                                                  pseudo_tok        tok,
                                                  add_fn            fn,
                                                  rprt_f     *const err_f ) {   // section segment parse
    cit10a_asrt(sptr != nullptr && map != nullptr);
    cit10a_asrt(org != nullptr && *org >= 0);
    cit10a_asrt(err_f != nullptr);

    // set flag
    bool        ret         =   false;

    // segment section
    do {
        // get first character
        for (; is_whitespace(*sptr->str); inc_strptr(sptr));
        err_f->ln   =   sptr->ln;

        // skip blank lines
        if (*sptr->str == CMMT_CHR || *sptr->str == '\0')   continue;

        // use lookup fn
        if (*sptr->str != PSEUDO_STRT) {
            if (fn(sptr, org, map, err_f))          ret =   true;
            continue;
        }

        // check pseudo-op
        const   tok_itm tok_lu  =   pseudo_hash_lu_adj_tok(sptr);
        if (tok_lu.grp == pseudo_segspec_t) {
            // break out of this (incremented in returned loop)
            if ((pseudo_tok)tok_lu.tok != tok)          return  false;

            // verify && skip
            if      (verify_sctn_strt(sptr, err_f))     ret =   true;
            else if (sptr->str == nullptr)              return  ret;
            continue;
        }

        switch (tok_lu.tok) {
            case tok_org:
                // new org
                const   int n_org   =   parse_org(sptr, err_f);
                if (n_org == -1)        return  true;
                *org                =   n_org;
                break;
            case tok_incl:
                // new file
                const   src_f   *const  sourc_f =   get_inc_static(sptr, srcs);
                ret                             =   commonseg(org, sourc_f, srcs, tok, fn, map);
                break;
            default:
                // handled elsewhere (hopefully)
                break;
        }
    } while (!newln_strptr(sptr, err_f->file));

    return  ret;
}

/**
 * Creates the lookup table of headers, and finds the total number of statements.
 *
 * @param       org             code start location
 * @param       source          source file
 * @param       srcs            sources
 * @param       tok             match token
 * @param       fn              inner add function call
 * @param       map             headermap or stackmap
 * @return                      whether an error occurred
 */
[[nodiscard]] bool commonseg(       int         *const org,
                              const src_f       *const source,
                              const sources     *const srcs,
                              const pseudo_tok         tok,
                                    add_fn             fn,
                                    void        *const map     ) {              // header stackmap creation
    cit10a_asrt(source != nullptr);
    cit10a_asrt(map != nullptr);

    // initialize header items
    bool        err     =   false;

    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check for section start
        const   pseudo_tok  pseudo_op       =   pseudo_hash_lu_adj(&sptr);
        if (pseudo_op == tok_incl) {
            const   src_f   *const  sourc_f =   get_inc_static(&sptr, srcs);
            err                             =   commonseg(org, sourc_f, srcs, tok, fn, map);
        }
        if (pseudo_op != tok)       continue;

        // check section line
        rprt_f  err_f   =   (rprt_f){ .file=source, .ln=sptr.ln, .col=sptr.col};
        if (verify_sctn_strt(&sptr, &err_f))                            err =   true;
        if (sptr.str == nullptr)                                        break;

        // parse section
        if (commonseg_parse_(&sptr, org, map, srcs, tok, fn, &err_f))   err =   true;
        i               =   sptr.ln;
    }

    // return populated constants
    return  err;
}

/*-VARIABLE-TOKEN-PRINTER---------------------------------------------------------------------------------------------*/

/**
 * Prints the var_tok header.
 *
 * @param       var             variable token header
 */
void print_var_tok(const var_tok *const var) {                                  // var_tok printer
    cit10a_asrt(var != nullptr);

    print_smap_head(&var->head);
    printf("[%s::%zu::%zu]", var->source->f_name, var->ln, var->col);
}
