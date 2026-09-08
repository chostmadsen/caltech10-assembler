/*
 * src/segmenter/segment.c
 * Segmenter orchestrator.
 */

#include    <stddef.h>
#include    <string.h>
#include    <inttypes.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/argparse.h"
#include    "datastructures/hash.h"
#include    "datastructures/stackmap.h"
#include    "common/gen_parse.h"
#include    "segmenter/segment.h"

/*-PARSER-HELPERS-----------------------------------------------------------------------------------------------------*/

/**
 * Gets the alphanumeric identifier (first character must be alphanumeric). Consumes it with the given strptr.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      stackmap head item
 */
[[nodiscard]] smap_head get_identifier( strptr *const sptr,
                                        rprt_f *const err_f ) {                 // identifier getter
    src_slice           slc         =   { .str=sptr->str, .len=0 };
    if (!is_alpha(*sptr->str)) {
        // invalid identifier
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid identifier", .report_f=err_f },
                    "invalid identifier"                                                           );
        return  (smap_head){ .hash=0, .key.str=nullptr, .key.len=0 };
    }

    // get and return identifier
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);
    return  (smap_head){ .hash=hash_fnv1a_slc_lwr(&slc), .key=slc };
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
    // find consptr
    while (is_whitespace(*sptr->str))   inc_strptr(sptr);

    // try to parse number
    if (('0' <= *sptr->str && *sptr->str <= '9') || *sptr->str == HEX_CHR_ALT) {
        const   int     ret         =   parse_num(sptr, err_f);
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

    if (clsn_t == smap_lwr_clsn_t && !c_args.case_sens) {
        // get old def
        char                    key[head->head.key.len + 1];
        memcpy(key, redef->head.key.str, head->head.key.len);
        key[head->head.key.len]         =   '\0';

        // case collision
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="case-variant redefinition", .report_f=err_f },
                    "case-variant redefinition of identifier [ %s @ %d::%d-%d ]", key, ln, col, col_e     );
        return  true;
    } else if (clsn_t == smap_full_clsn_t){
        // full collision
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="redefinition", .report_f=err_f },
                "redefinition of identifier [ @ %d::%d-%d ]", ln, col, col_e             );
        return  true;
    }
    return  false;
}

/*-VARIABLE-TOKEN-PRINTER---------------------------------------------------------------------------------------------*/

/**
 * Prints the var_tok header.
 *
 * @param       var             variable token header
 */
void print_var_tok(const var_tok *const var) {                                  // var_tok printer
    cit10a_asrt(var != nullptr);

    printf("0x%016" PRIx64 " : ", var->head.hash);
    print_src_slice(&var->head.key, stdout);
    printf("[%zu::%zu]", var->ln, var->col);
}
