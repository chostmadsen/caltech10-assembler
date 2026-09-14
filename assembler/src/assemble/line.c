/**
 * src/assemble/line.c
 * Single line assembly.
 */

#include    <stddef.h>
#include    <stdio.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stackmap.h"
#include    "common/kwrds.h"
#include    "common/hash_tables/opcode.h"
#include    "common/gen_parse.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "segmenter/constseg.h"
#include    "segmenter/dataseg.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"

/*-LINE-ASSEMBLY-VALUE-GETTERS----------------------------------------------------------------------------------------*/

/**
 * Get assembly value, either from a raw value or the constant map.
 *
 * @param       sptr            string pointer
 * @param       constmap        constants
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int asm_get_val_(        strptr   *const sptr,
                                       const  stackmap *const constmap,
                                       rprt_f          *const err_f     ) {     // assembler value
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(constmap != nullptr);

    if (is_num_strt(*sptr->str)) {
        // raw number parse
        return  parse_num_repr(sptr, err_f);
    }

    // constant lookup
    src_slice   slc =   { .str=sptr->str, .len=0 };
    const   int col =   sptr->col;
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);

    if (slc.len == 0) {
        err_f->col  =   col;
        err_f->len  =   1;
        cit10a_msg(&(msg_info){ .type=msg_err_t, .header="missing value", .report_f=err_f }, "missing value");
        return  -1;
    }

    const   const_var  *cvar;
    if    (c_args.case_sens)    cvar    =   stackmap_get_k_lwr(constmap, &slc);
    else                        cvar    =   stackmap_get_k_lwr_lwr(constmap, &slc);
    if (cvar == nullptr) {
        err_f->col  =   col;
        err_f->len  =   slc.len;
        cit10a_msg(&(msg_info){ .type=msg_err_t, .header="unknown constant", .report_f=err_f }, "undefined constant");
        return  -1;
    }
    return  cvar->val;
}

/**
 * Get an immediate value, for instructions that require an immediate value.
 *
 * @param       sptr            string pointer
 * @param       constmap        constants
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int get_immediate_(        strptr   *const sptr,
                                         const  stackmap *const constmap,
                                         rprt_f          *const err_f     ) {     // assembler immediate
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(constmap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // get immediate value
    const   int     imd_v       =   asm_get_val_(sptr, constmap, err_f);
    if (imd_v == -1)                return  -1;
    if (check_ln_end(sptr, err_f))  return  -1;
    return  imd_v;
}

/*-LINE-ASSEMBLY-MEMORY-GETTERS---------------------------------------------------------------------------------------*/

/**
 * Get a value from the defined program memory.
 *
 * @param       sptr            string pointer
 * @param       datamap         memory
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int get_mem_(        strptr   *const sptr,
                                   const  stackmap *const datamap, 
                                   rprt_f          *const err_f    ) {          // assembler memory
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(datamap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    int     ret;
    if (is_num_strt_ns(*sptr->str)) {
        // raw number parse
        err_f->len  =   (*sptr->str == HEX_CHR_ALT) ? 1 : 0;
        for (; is_alphanum(sptr->str[err_f->len]); ++err_f->len);
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="raw memory access", .report_f=err_f },
                    "raw memory address access; define memory in the `.data` section"              );
        ret         =   parse_num_repr(sptr, err_f);
    } else {
        // memory lookup
        src_slice   slc =   { .str=sptr->str, .len=0 };
        const   int col =   sptr->col;
        for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);
        if (slc.len == 0) {
            err_f->col  =   col;
            err_f->len  =   1;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="missing identifier", .report_f=err_f },
                        "missing identifier"                                                           );
            return  -1;
        }

        // table lookup
        const   data_var   *dvar;
        if    (c_args.case_sens)    dvar    =   stackmap_get_k_lwr(datamap, &slc);
        else                        dvar    =   stackmap_get_k_lwr_lwr(datamap, &slc);
        if (dvar == nullptr) {
            err_f->col  =   col;
            err_f->len  =   slc.len;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="unknown data", .report_f=err_f },
                        "undefined memory location"                                              );
            return  -1;
        }
        ret             =   dvar->loc;
    }

    if (check_ln_end(sptr, err_f))  return  -1;
    return  ret;
}

/*-LINE-ASSEMBLY-OFFSET-GETTERS---------------------------------------------------------------------------------------*/

/**
 * Get an offset from some addressing modes.
 *
 * @param       sptr            string pointer
 * @param       constmap        constants
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int get_offset_(        strptr   *const sptr,
                                      const  stackmap *const constmap,
                                      rprt_f          *const err_f     ) {      // offset
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(constmap != nullptr);

    // check for offset
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str != CMMA_CHR) {
        if (*sptr->str == '\0' || *sptr->str == CMMT_CHR)   return  0;

        // trailing characters
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="trailing characters", .report_f=err_f},
                    "invalid offset start (possibly misplaced `,` character)"                      );
        return  -1;
    }

    // get offset
    for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
    const   int     ret         =   asm_get_val_(sptr, constmap, err_f);
    if (ret == -1)                  return  -1;
    if (check_ln_end(sptr, err_f))  return  -1;
    return  ret;
}

/**
 * Get the full addressing mode adjustment from an ALU instruction.
 *
 * @param       sptr            string pointer
 * @param       segmap          segmaps
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int alu_offs_(        strptr  *const sptr,
                                    const  segmaps *const segmap, 
                                    rprt_f         *const err_f   ) {           // alu addressing mode select
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(segmap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    int         ret     =   0;

    // get addressing mode
    if      (to_lwr_chr(*sptr->str) == X_REG_CHR)           ret +=  ALU_X_ADRS;
    else if (to_lwr_chr(*sptr->str) == S_REG_CHR)           ret +=  ALU_S_ADRS;
    else                                                    return  get_mem_(sptr, &segmap->datamap, err_f);
    inc_strptr(sptr);

    // get offset
    const   int offs    =   get_offset_(sptr, &segmap->constmap, err_f);
    if (offs == -1)         return  -1;
    return  ret + offs;
}

/**
 * Get the full addressing mode adjustment from a load / store instruction.
 *
 * @param       sptr            string pointer
 * @param       segmap          segmaps
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int ldst_offs_(        strptr  *const sptr,
                                     const  segmaps *const segmap, 
                                     rprt_f         *const err_f   ) {          // load store addressing mode select
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(segmap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    int         ret     =   0;

    // get addressing mode
    err_f->col          =   sptr->col;
    bool        adjst   =   false;
    bool        err     =   false;
    if (*sptr->str == REG_DEC_CHR || *sptr->str == REG_INC_CHR) {
        adjst           =   true;
        ret             +=  LDST_PRE;
        ret             +=  (*sptr->str == REG_DEC_CHR) ? LDST_DEC : LDST_INC;
        inc_strptr(sptr);
    }
    if      (to_lwr_chr(*sptr->str) == X_REG_CHR)           { ret +=  LDST_X_ADRS;  inc_strptr(sptr); }
    else if (to_lwr_chr(*sptr->str) == S_REG_CHR)           { ret +=  LDST_S_ADRS;  inc_strptr(sptr); }
    else                                                    err =   true;
    if (*sptr->str == REG_DEC_CHR || *sptr->str == REG_INC_CHR) {
        if (adjst)      err =   true;
        else {
            ret             +=  LDST_POST;
            ret             +=  (*sptr->str == REG_DEC_CHR) ? LDST_DEC : LDST_INC;
            inc_strptr(sptr);
        }
    }

    if (err) {
        err_f->len  =   (sptr->col - err_f->col > 0) ? sptr->col - err_f->col : 1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid addressing mode", .report_f=err_f},
                    "invalid load / store addressing mode"                                             );
        return  -1;
    }

    // get offset
    const   int offs    =   get_offset_(sptr, &segmap->constmap, err_f);
    if (offs == -1)         return  -1;
    return  ret + offs;
}

/*-LINE-ASSEMBLY-JUMP-PARSERS-----------------------------------------------------------------------------------------*/

/**
 * Jump parser; returns the absolute location of a jump, even for relative jumps from the program counter.
 *
 * @param       sptr            string pointer
 * @param       headermap       headers
 * @param       loc             current location
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int jmp_parse_(       strptr   *const sptr,
                                     const stackmap *const headermap, 
                                     const int             loc, 
                                           rprt_f   *const err_f      ) {       // jump parser
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(headermap != nullptr);
    cit10a_asrt(loc >= 0);

    // return setup
    int             ret     =   0;

    // check for relative
    if (*sptr->str == PC_CHR) {
        err_f->col      =   sptr->col;

        // check next character
        for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
        if (*sptr->str == '\0' || *sptr->str == CMMT_CHR) {
            ret         =   loc;
            goto    ret_chck;
        }

        if (*sptr->str != NEG_SYMB && *sptr->str != POS_SYMB) {
            // invalid modification
            err_f->col  =   sptr->col;
            err_f->len  =   1;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid pc modification operation", err_f }, 
                        "pc modification must be an arithmetic operation"                                   );
            return  -1;
        }

        // get jump
        bool        neg =   (*sptr->str == NEG_SYMB) ? true : false;
        for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
        const   int adj =   parse_num(sptr, err_f);
        ret             =   (neg) ? loc - adj : loc + adj;

    } else if (is_num_strt_ns(*sptr->str)) {

        err_f->col  =   sptr->col;
        // raw number parse
        err_f->len  =   (*sptr->str == HEX_CHR_ALT) ? 1 : 0;
        for (; is_alphanum(sptr->str[err_f->len]); ++err_f->len);
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="raw program access", .report_f=err_f },
                    "raw program access; define headers or set up a relative jump with `.`"         );
        ret         =   parse_num_adrs(sptr, err_f);
        if (ret == -1)  return  -1;

    } else {

        err_f->col  =   sptr->col;
        // header lookup
        src_slice   slc =   { .str=sptr->str, .len=0 };
        const   int col =   sptr->col;
        for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);
        if (slc.len == 0) {
            err_f->col  =   col;
            err_f->len  =   1;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="missing identifier", .report_f=err_f },
                        "missing identifier"                                                           );
            return  -1;
        }

        const   header_var *hvar;
        if    (c_args.case_sens)    hvar    =   stackmap_get_k_lwr(headermap, &slc);
        else                        hvar    =   stackmap_get_k_lwr_lwr(headermap, &slc);
        if (hvar == nullptr) {
            err_f->col  =   col;
            err_f->len  =   slc.len;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="unknown header", .report_f=err_f },
                        "undefined program location"                                               );
            return  -1;
        }
        ret             =   hvar->loc;
        cit10a_asrt(ret >= 0);
    }

ret_chck:
    // check line end
    if (check_ln_end(sptr, err_f))      return  -1;
    if (ret < 0 || ret > (int)MAX_ADRS) {
        err_f->len  =   sptr->col - err_f->col;
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="invalid jump", err_f },
                "wrapped jump to inside of program memory (originally to %c0x%x)",
                (ret < 0) ? '-' : '+', (ret < 0) ? -ret : ret                       );
        return  ret & MAX_ADRS;
    }
    return  ret;
}

/**
 * Relative jump parser; relative from current offset, minus 1 (in accordance with how the CPU works).
 *
 * @param       sptr            string pointer
 * @param       headermap       headers
 * @param       loc             current location
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int rjmp_(       strptr   *const sptr,
                                const stackmap *const headermap, 
                                const int             loc, 
                                      rprt_f   *const err_f      ) {            // relative jump
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(headermap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    const   size_t  p_strt  =   sptr->col;

    // get jump location
    const   int jloc    =   jmp_parse_(sptr, headermap, loc, err_f);
    if (jloc == -1)         return  -1;
    // signed conversion
    int         rel     =   (jloc - loc - 1) & MAX_ADRS;
    rel                 =   (rel >= (int)(MAX_ADRS + 1) >> 1) ? rel - MAX_ADRS - 1 : rel;

    // range check
    if (rel < -(int)MAX_NUM_NEG || rel > (int)MAX_NUM_POS) {
        // jump range oob
        err_f->col      =   p_strt;
        err_f->len      =   sptr->col - p_strt;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="relative jump range", .report_f=err_f },
                    "relative jump out of range (%c0x%04x + 1)", (rel < 0) ? '-' : '+', (rel < 0) ? -rel : rel );
        return  -1;
    }

    // overflow check
    const   int rel_r   =   loc + 1 + rel;
    if (!c_args.warnings.nwrapj && (rel_r < 0 || rel_r > (int)MAX_ADRS)) {
        // program counter wrapping
        err_f->col      =   p_strt;
        err_f->len      =   sptr->col - p_strt;
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="implicit relative jump wrap", .report_f=err_f},
                    "relative jump uses program counter overflow; this may be UB"                           );
    }
    return  rel & MAX_NUM;
}

/**
 * Absolute jump parser; absolute program address.
 *
 * @param       sptr            string pointer
 * @param       headermap       headers
 * @param       loc             current location
 * @param       err_f           error file
 * @return                      value (-1 for error)
 */
[[nodiscard]] static int ajmp_(       strptr   *const sptr,
                                const stackmap *const headermap, 
                                const int             loc, 
                                      rprt_f   *const err_f) {                  // absolute jump
    cit10a_asrt(sptr != nullptr);
    cit10a_asrt(headermap != nullptr);

    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    return  jmp_parse_(sptr, headermap, loc, err_f);
}

/*-LINE-ASSEMBLER-----------------------------------------------------------------------------------------------------*/

/**
 * Assembles a singular line, returning the full instruction as well as the line.
 *
 * @param       segmap          segmaps
 * @param       ln_inf          line information
 * @return                      line assembly instruction. .ln=-1 for error.
 */
[[nodiscard]] ln_asm line_assemble( const segmaps *const segmap, 
                                    const ln_info *const ln_inf  ) {            // line assembler
    cit10a_asrt(segmap != nullptr);
    cit10a_asrt(ln_inf != nullptr);
    cit10a_asrt(ln_inf->loc >= 0);

    const   src_f   *const  source  =   ln_inf->source;
    strptr                  sptr    =   { .ln=ln_inf->ln, .col=0, .str=src_f_getline(source, ln_inf->ln) };
    for (; is_whitespace(*sptr.str); inc_strptr(&sptr));

    // get code
    size_t  n       =   0;
    for (; is_alphanum(sptr.str[n]); ++n);
    if (sptr.str[n] == HEADER_CHR) {
        // skip header
        adj_strptr(&sptr, n + 1);
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        n       =   0;
        for (; is_alphanum(sptr.str[n]); ++n);
    }

    tok_itm     op_tok  =   opcode_hash_lu(sptr.str, n);
    ln_asm      ret     =   {{ .instr=op_tok.instr, .loc=ln_inf->loc, .source=source, .ln=ln_inf->ln }};
    rprt_f      err_f   =   { .file=source, .ln=ln_inf->ln, .col=sptr.col };
    adj_strptr(&sptr, n);

    switch(op_tok.grp) {

        case opcode_no_grp:
            // no opcode
            err_f.len   =   (n > 0) ? n : 1;
            cit10a_msg(&(msg_info){ .type=msg_err_t, .header="unknown opcode", .report_f=&err_f}, "invalid opcode");
            return  (ln_asm){{ .ln=-1, .instr=-1 }};

        case alu_offset_t:
            // alu offset
            const   int     alu_v   =   alu_offs_(&sptr, segmap, &err_f);
            if (alu_v == -1)            return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  alu_v;
            break;

        case io_t:              [[fallthrough]];
        case ldst_immediate_t:  [[fallthrough]];
        case alu_immediate_t:
            // any immediate
            const   int     imd_v   =   get_immediate_(&sptr, &segmap->constmap, &err_f);
            if (imd_v == -1)            return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  imd_v;
            break;

        case alu_lone_t:        [[fallthrough]];
        case flag_t:            [[fallthrough]];
        case subrout_st_lone_t: [[fallthrough]];
        case misc_t:            [[fallthrough]];
        case ind_reg_t:
            // any lone operation
            if (check_ln_end(&sptr, &err_f))    return  (ln_asm){{ .ln=-1, .instr=-1 }};
            break;

        case ldst_direct_t:
            // load store direct
            const   int     dir_v   =   get_mem_(&sptr, &segmap->datamap, &err_f);
            if (dir_v == -1)            return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  dir_v;
            break;

        case ldst_indexed_t:
            // load store indexed
            const   int     ldst_v  =   ldst_offs_(&sptr, segmap, &err_f);
            if (ldst_v == -1)           return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  ldst_v;
            break;

        case jmp_relative_t:
            // relative jump
            const   int     rjmp_v  =   rjmp_(&sptr, &segmap->headmap.smap, ln_inf->loc, &err_f);
            if (rjmp_v == -1)           return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  rjmp_v;
            break;

        case jmp_absolute_t:    [[fallthrough]];
        case subrout_st_adrs_t:
            // absolute pc adjustment
            const   int     ajmp_v  =   ajmp_(&sptr, &segmap->headmap.smap, ln_inf->loc, &err_f);
            if (ajmp_v == -1)           return  (ln_asm){{ .ln=-1, .instr=-1 }};
            ret.la.instr            +=  ajmp_v;
            break;

        default:
            cit10a_asrt(!"opcode token oob");
            cit10a_exit(INTRNL_ERRNO);

    }

    return  ret;
}
