/**
 * src/assemble/assemble_emit.c
 * Emitter for the assembled assembly.
 */

#include    <stddef.h>
#include    <stdio.h>

#include    "helpers/general.h"
#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stackmap.h"
#include    "common/hash_tables/pseudo.h"
#include    "common/kwrds.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"
#include    "assemble/assemble.h"
#include    "assemble/assemble_emit.h"

/*-ASSEMBLY-EMITTER-HELPERS-------------------------------------------------------------------------------------------*/

/**
 * Get the first non-whitespace character of a string pointer and place the string pointer there.
 * 
 * @param       sptr            string pointer
 * @return                      first non-whitespace character
 */
[[nodiscard]] static char first_chr_(strptr *const sptr) {                      // first non-whitespace character
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    return  *sptr->str;
}

/**
 * Get the string slice of the specified header, if only the header is present on that line.
 * Otherwise, src_slice.str == nullptr.
 *
 * @param       sptr            string pointer to first character
 * @return                      header slice
 */
[[nodiscard]] static src_slice header_get_(strptr *const sptr) {                // header string slice
    // get identity slice
    src_slice   ident   =   { .str=sptr->str, .len=0 };
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++ident.len);

    // verify only header
    if (*sptr->str != HEADER_CHR)                       return  (src_slice){ .str=nullptr, .len=0 };
    for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str == CMMT_CHR || *sptr->str == '\0')   return  ident;
    return  (src_slice){ .str=nullptr, .len=0 };
}

/**
 * Pseudo-op lookup on a string.
 *
 * @param       str             string
 * @return                      lookup token
 */
[[nodiscard]] static pseudo_tok get_pseudo_(const char *str) {                  // pseudo-op lookup
    for (; is_whitespace(*str); ++str);
    cit10a_asrt(*str == PSEUDO_STRT);

    const   char    *const  lu_str  =   ++str;
    size_t                  len     =   0;
    for (; is_alphanum(*str); ++str, ++len);
    const   pseudo_tok      ret     =   pseudo_hash_lu(lu_str, len).tok;
    cit10a_asrt(ret != pseudo_no_tok);
    return  ret;
}

/**
 * Assembly singular instruction emitter.
 * Pass in instr=-1 to omit instruction (header only) and ln=nullptr to omit attached line.
 *
 * @param       fp              output file
 * @param       loc             instruction location
 * @param       instr           instruction
 * @param       ln              attached line
 */
static void asm_emit_instr_(       FILE *const fp,
                             const int         loc,
                             const int         instr,
                             const char *const ln     ) {                       // assembly instruction emitter
    // start
    if      (instr == -1)           fputs(ADRS_STRT, fp);
    else if (ln != nullptr)         fputs(CODE_STRT, fp);
    fprintf(fp, "%02x" SEG_SEP_ADRS "%02x", (loc & UPPER_AND) >> BYTE_S, (loc & LOWER_AND));

    if (instr == -1) {
        cit10a_asrt(ln != nullptr);
        // instruction only
        fprintf(fp, ADRS_END "%s\n", ln);
        return;
    }

    // full code
    fprintf(fp, CODE_SEP "%02x" SEG_SEP_INSTR "%02x", (instr & UPPER_AND) >> BYTE_S, (instr & LOWER_AND));
    if (ln != nullptr)              fprintf(fp, CODE_END "%s", ln);
    fputc('\n', fp);
}

/*-ASSEMBLY-SECTION-EMITTERS------------------------------------------------------------------------------------------*/

/**
 * .data section emitter for a file.
 *
 * @param       fp              output file
 * @param       start           file start location
 * @param       source          source file
 * @param       datamap         data lookup map
 * @return                      ended location (-1 for EOF)
 */
[[nodiscard]] static size_t emit_asm_data_(       FILE     *const fp,     
                                            const size_t          strt,
                                            const src_f    *const source,
                                            const stackmap *const datamap ) {   // assembly data emitter
    cit10a_asrt(fp != nullptr);
    cit10a_asrt(source != nullptr);
    cit10a_asrt(datamap != nullptr);

    for (size_t i = strt; i < source->ln_num; ++i) {
        const   char    *const  ln          =   src_f_getline(source, i);
        strptr                  sptr        =   { .str=ln, .ln=-1, .col=0 };
        const   char            frst_chr    =   first_chr_(&sptr);

        // special lookup
        if      (frst_chr == '\0')              { fputc('\n', fp); continue; }
        else if (frst_chr == CMMT_CHR)          { fprintf(fp, FULL_OUT "%s\n", ln); continue; }
        else if (frst_chr == PSEUDO_STRT)       { switch (get_pseudo_(ln)) {
            case tok_org:   [[fallthrough]];
            case tok_const: [[fallthrough]];
            case tok_data:
                fprintf(fp, CMMT_STRT "%s\n", ln);
                break;
            case tok_incl:  cit10a_asrt(!"invalid for now");
            default:        return  i;
        } continue; }

        // data lookup
        src_slice   ident   =   { .str=sptr.str, .len=0 };
        for (; is_alphanum(*sptr.str); inc_strptr(&sptr), ++ident.len);

        const   header_var  *const  dt  =   (c_args.case_sens)
                                            ? (header_var*)stackmap_get_k_lwr(datamap, &ident)
                                            : (header_var*)stackmap_get_k_lwr_lwr(datamap, &ident);
        cit10a_asrt(dt != nullptr);
        fprintf(fp, DATA_STRT "%02x" DATA_END "%s\n", dt->loc, ln);
    }
    return  -1;
}

/**
 * .data section emitter for a file.
 *
 * @param       fp              output file
 * @param       start           file start location
 * @param       source          source file
 * @param       headmap         header lookup map
 * @param       asm_r           assembly
 * @return                      ended location (-1 for EOF)
 */
[[nodiscard]] static size_t emit_asm_code_(       FILE     *const fp,     
                                            const size_t          strt,
                                            const src_f    *const source,
                                            const stackmap *const headmap,
                                            const asm_ret  *const asm_r    ) {  // assembly data emitter
    cit10a_asrt(fp != nullptr);
    cit10a_asrt(source != nullptr);
    cit10a_asrt(headmap != nullptr);
    cit10a_asrt(asm_r != nullptr);

    // assembly reference
    int     asm_src     =   0;
    int     src_loc     =   asm_r->ln_asms[0].ln;

    for (size_t i = strt; i < source->ln_num; ++i) {
        const   char    *const  ln          =   src_f_getline(source, i);
        strptr                  sptr        =   { .str=ln, .ln=-1, .col=0 };
        const   char            frst_chr    =   first_chr_(&sptr);

        // special lookup
        if      (frst_chr == '\0')              { fputc('\n', fp); continue; }
        else if (frst_chr == CMMT_CHR)          { fprintf(fp, FULL_OUT "%s\n", ln); continue; }
        else if (frst_chr == PSEUDO_STRT)       { switch (get_pseudo_(ln)) {
            case tok_org:   [[fallthrough]];
            case tok_const: [[fallthrough]];
            case tok_code:
                fprintf(fp, CMMT_STRT "%s\n", ln);
                break;
            case tok_incl:  cit10a_asrt(!".include invalid for now");
            default:        return  i;
        } continue; }

        // only header
        const   src_slice   head    =   header_get_(&sptr);
        if (head.str != nullptr) {
            const   header_var  *const  hd  =   (c_args.case_sens)
                                                ? (header_var*)stackmap_get_k_lwr(headmap, &head)
                                                : (header_var*)stackmap_get_k_lwr_lwr(headmap, &head);
            cit10a_asrt(hd != nullptr);
            asm_emit_instr_(fp, hd->loc, -1, ln);
            continue;
        }

        // get code
        cit10a_asrt(asm_r->num_segs != 0);
        for (; src_loc != (int)i; src_loc = asm_r->ln_asms[++asm_src].ln) {
            cit10a_asrt(asm_src + 1 < asm_r->num_segs);
        }
        const   ln_asm  asm_l   =   asm_r->ln_asms[asm_src];
        asm_emit_instr_(fp, asm_l.loc, asm_l.instr, ln);
    }
    return  -1;
}

/*-FULL-ASSEMBLY-EMITTER----------------------------------------------------------------------------------------------*/

/**
 * Assembly emitter to a file.
 *
 * @param       f_name          output file name
 * @param       source          source file
 * @param       segmap          lookup maps
 * @param       asm_r           assembly
 */
void emit_asm( const char    *const f_name, const src_f   *const source,
               const segmaps *const segmap, const asm_ret *const asm_r   ) {    // assembly emitter
    cit10a_asrt(f_name != nullptr);
    cit10a_asrt(source != nullptr);
    cit10a_asrt(segmap != nullptr);
    cit10a_asrt(asm_r != nullptr);

    // open file
    FILE   *const           fp      =   fopen(f_name, "wb");
    if (fp == nullptr) {
        const   rprt_f      err_f   =   { .file=&(src_f){ .f_name=(char*)f_name }, .len=0 };
        // invalid file
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="file error", .report_f=&err_f },
                    "couldn't open / read output file"                                      );
        cit10a_exit(OUTPUT_ERRNO);
    }

    if (c_args.bin) {
        // only binary
        for (int i = 0; i < asm_r->num_segs; ++i) {
            asm_emit_instr_(fp, asm_r->ln_asms[i].loc, asm_r->ln_asms[i].instr, nullptr);
        }
        return;
    }

    for (size_t i = 0; i < source->ln_num; ++i) {
        // iterate over .none
        const   char    *const  ln          =   src_f_getline(source, i);
        strptr                  sptr        =   { .str=ln, .ln=-1, .col=0 };
        const   char            frst_chr    =   first_chr_(&sptr);

        // possible new section
        if      (frst_chr == '\0')              { fputc('\n', fp); continue; }
        else if (frst_chr == CMMT_CHR)          { fprintf(fp, "%s\n", ln); continue; }
        else if (frst_chr == PSEUDO_STRT)       { switch (get_pseudo_(ln)) {
            case tok_org:   [[fallthrough]];
            case tok_const: [[fallthrough]];
            case tok_none:
                fprintf(fp, CMMT_STRT "%s\n", ln);
                break;
            case tok_code:
                const size_t    c_ret   =   emit_asm_code_(fp, i, source, &segmap->headmap.smap, asm_r);
                if (c_ret == (size_t)-1)    goto    debug_emit;
                i                       =   c_ret - 1;
                break;
            case tok_data:
                const size_t    d_ret   =   emit_asm_data_(fp, i, source, &segmap->datamap);
                if (d_ret == (size_t)-1)    goto    debug_emit;
                i                       =   d_ret - 1;
                break;
            case tok_incl:  cit10a_asrt(!"invalid for now");
            default:        cit10a_asrt(!"invalid pseudo-op state");
        } continue; }
    }

debug_emit:
    // debug information
    fputs("\n\n", fp);
    fprintf(fp, VERS_STRT "%d words | ", asm_r->num_segs);
    cit10a_version_f(fp, false);
    fputc('\n', fp);
}
