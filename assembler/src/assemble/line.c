/**
 * src/assemble/line.c
 * Single line assembly.
 */

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

[[nodiscard]] static int asm_get_val(strptr *const sptr, const stackmap *const constmap, rprt_f *const err_f) {
    if (('0' <= *sptr->str && *sptr->str <= '9') || *sptr->str == HEX_CHR_ALT) {
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

[[nodiscard]] static int get_immediate_(strptr *const sptr, const stackmap *const constmap, rprt_f *const err_f) {
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // get immediate value
    const   int     imd_v       =   asm_get_val(sptr, constmap, err_f);
    if (imd_v == -1)                return  -1;
    if (check_ln_end(sptr, err_f))  return  -1;
    return  imd_v;
}

[[nodiscard]] static int get_mem_(strptr *const sptr, const stackmap *const datamap, rprt_f *const err_f) {
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    int     ret;
    if (('0' <= *sptr->str && *sptr->str <= '9') || *sptr->str == HEX_CHR_ALT) {
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

[[nodiscard]] static int get_offset_(strptr *const sptr, const stackmap *const constmap, rprt_f *const err_f) {
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
    const   int     ret         =   asm_get_val(sptr, constmap, err_f);
    if (ret == -1)                  return  -1;
    if (check_ln_end(sptr, err_f))  return  -1;
    return  ret;
}

[[nodiscard]] static int alu_offs_(strptr *const sptr, const segmaps *const segmap, rprt_f *const err_f) {
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

[[nodiscard]] static int ldst_offs_(strptr *const sptr, const segmaps *const segmap, rprt_f *const err_f) {
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

[[nodiscard]] static int jmp_parse_(strptr *const sptr, const stackmap *const headermap, rprt_f *const err_f, const int loc) {
    // return setup
    int             ret     =   0;

    // check for relative
    if (*sptr->str == PC_CHR) {
        for (inc_strptr(sptr); is_whitespace(*sptr->str); inc_strptr(sptr));
        if (*sptr->str == '\0' || *sptr->str == CMMT_CHR)   return  ret;

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
        inc_strptr(sptr);
        err_f->col      =   sptr->col;
        const   int adj =   parse_num(sptr, err_f);
        ret             =   (neg) ? loc - adj : loc + adj;

    } else if (('0' <= *sptr->str && *sptr->str <= '9') || *sptr->str == HEX_CHR_ALT) {

        // raw number parse
        err_f->len  =   (*sptr->str == HEX_CHR_ALT) ? 1 : 0;
        for (; is_alphanum(sptr->str[err_f->len]); ++err_f->len);
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="raw program access", .report_f=err_f },
                    "raw program access; define headers or set up a relative jump with `.`"         );
        ret         =   parse_num_repr(sptr, err_f);

    } else {

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

    }

    // check line end
    if (check_ln_end(sptr, err_f))      return  -1;
    if (ret < 0 || ret > (int)MAX_ADRS) {
        err_f->len  =   sptr->col - err_f->col;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid jump", err_f }, 
                "jump to outside of program memory"                                );
        return  -1;
    }
    return  ret;
}

[[nodiscard]] static int rjmp_(strptr *const sptr, const stackmap *const headermap, rprt_f *const err_f, const int loc) {
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    size_t  p_strt      =   sptr->col;

    // get jump location
    const   int jloc    =   jmp_parse_(sptr, headermap, err_f, loc);
    if (jloc == -1)         return  -1;
    int         ret     =   jloc - loc;

    // check jump range
    bool    neg         =   false;
    int     max_num     =   MAX_NUM_POS;
    if (ret < 0) {
        neg             =   true;
        max_num         =   MAX_NUM_NEG;
        ret             =   -ret;
    }
    if (ret > max_num) {
        err_f->col      =   p_strt;
        err_f->len      =   sptr->col - p_strt;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="relative jump range", .report_f=err_f },
                    "relative jump out of range (maximially %d-bit, signed 2's compliment)", MAX_NUM_PARSE );
        return  -1;
    }

    return  (neg) ? (ret ^ MAX_NUM) + 1 : ret;
}

[[nodiscard]] static int ajmp_(strptr *const sptr, const stackmap *const headermap, rprt_f *const err_f, const int loc) {
    // skip whitespace
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    return  jmp_parse_(sptr, headermap, err_f, loc);
}

[[nodiscard]] ln_asm line_assemble(const src_f *const source, const segmaps *const segmap, const ln_info *const ln_inf) {

    strptr  sptr    =   { .ln=ln_inf->ln, .col=0, .str=src_f_getline(source, ln_inf->ln) };
    for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
    const   char    *const  str_out_tst =   sptr.str;

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
    ln_asm      ret     =   { .instr=op_tok.instr, .ln=ln_inf->ln };
    rprt_f      err_f   =   { .file=source, .ln=ln_inf->ln, .col=sptr.col };
    adj_strptr(&sptr, n);

    switch(op_tok.grp) {

        case opcode_no_grp:
            err_f.len   =   n;
            cit10a_msg(&(msg_info){ .type=msg_err_t, .header="unknown opcode", .report_f=&err_f}, "invalid opcode");
            return  (ln_asm){ .ln=-1, .instr=-1 };

        case alu_offset_t:
            const   int     alu_v   =   alu_offs_(&sptr, segmap, &err_f);
            if (alu_v == -1)            return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  alu_v;
            break;

        case io_t:              [[fallthrough]];
        case ldst_immediate_t:  [[fallthrough]];
        case alu_immediate_t:
            const   int     imd_v   =   get_immediate_(&sptr, &segmap->constmap, &err_f);
            if (imd_v == -1)            return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  imd_v;
            break;

        case alu_lone_t:        [[fallthrough]];
        case flag_t:            [[fallthrough]];
        case subrout_st_lone_t: [[fallthrough]];
        case misc_t:            [[fallthrough]];
        case ind_reg_t:
            if (check_ln_end(&sptr, &err_f))    return  (ln_asm){ .ln=-1, .instr=-1 };
            break;

        case ldst_direct_t:
            const   int     dir_v   =   get_mem_(&sptr, &segmap->datamap, &err_f);
            if (dir_v == -1)            return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  dir_v;
            break;

        case ldst_indexed_t:
            const   int     ldst_v  =   ldst_offs_(&sptr, segmap, &err_f);
            if (ldst_v == -1)           return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  ldst_v;
            break;

        case jmp_relative_t:
            const   int     rjmp_v  =   rjmp_(&sptr, &segmap->headmap.smap, &err_f, ln_inf->loc);
            if (rjmp_v == -1)           return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  rjmp_v - 1;
            break;

        case jmp_absolute_t:    [[fallthrough]];
        case subrout_st_adrs_t:
            const   int     ajmp_v  =   ajmp_(&sptr, &segmap->headmap.smap, &err_f, ln_inf->loc);
            if (ajmp_v == -1)           return  (ln_asm){ .ln=-1, .instr=-1 };
            ret.instr               +=  ajmp_v - 1;
            break;

        default:
            cit10a_asrt(!"opcode token oob");
            cit10a_exit(INTRNL_ERRNO);

    }

    printf("%04x %04x :: %s\n", ln_inf->loc, ret.instr, str_out_tst);
    return  ret;
}
