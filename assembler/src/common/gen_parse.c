/*
 * src/common/gen_parse.c
 * General parser items.
 */

#include    <stddef.h>
#include    <stdlib.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "common/kwrds.h"
#include    "common/gen_parse.h"

/*-LINE-VERIFIERS-----------------------------------------------------------------------------------------------------*/

/**
 * At the string pointer, verify the line ends normally.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      whether the line ends abnormally
 */
[[nodiscard]] bool check_ln_end(strptr *const sptr, rprt_f *const err_f) {      // line end checker
    cit10a_asrt(sptr != nullptr && sptr->str != nullptr);
    cit10a_asrt(err_f != nullptr);

    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    if (*sptr->str != CMMT_CHR && *sptr->str != '\0') {
        err_f->ln   =   sptr->ln;
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="trailing character", .report_f=err_f },
                    "unexpected trailing character"                                                );
        return  true;
    }
    return  false;
}

/**
 * Gets the alphanumeric identifier (first character must be alphanumeric). Consumes it with the given strptr.
 *
 * @param       sptr            string pointer
 * @param       err_f           error report file
 * @return                      stackmap head item
 */
[[nodiscard]] src_slice get_ident(strptr *const sptr, rprt_f *const err_f) {    // identifier parser
    src_slice           slc         =   { .str=sptr->str, .len=0 };
    if (!is_alpha(*sptr->str)) {

        cit10a_asrt(err_f != nullptr);
        // invalid identifier
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid identifier", .report_f=err_f },
                    "invalid identifier"                                                           );
        return  slc;
    }

    // get and return identifier
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slc.len);
    return  slc;
}

/*-NUMBER-PARSERS-----------------------------------------------------------------------------------------------------*/

/**
 * Check of a character is a number start.
 *
 * @param       chr             character
 * @return                      whether the number starts
 */
[[nodiscard]] bool is_num_strt(const char chr) {                                // number start checker
    return  (chr == NEG_SYMB || chr == POS_SYMB || ('0' <= chr && chr <= '9') || chr == HEX_CHR_ALT || chr == CHR_CHR);
}

/**
 * Check of a character is a number start (non-signed)
 *
 * @param       chr             character
 * @return                      whether the number starts
 */
[[nodiscard]] bool is_num_strt_ns(const char chr) {                             // number start checker (non-signed)
    return  (('0' <= chr && chr <= '9') || chr == HEX_CHR_ALT || chr == CHR_CHR);
}

/**
 * Escape character ascii value. -1 if escape character is invalid
 *
 * @param       chr                 character
 * @return                          ascii value
 */
[[nodiscard]] static int esc_chr_val_(const char chr) {                         // escape character value
    switch (chr) {
#define X( chr, val )   case chr:   return  (int)val;
        ESC_CHRS
#undef  X
        default:    return  -1;
    }
}

/**
 * Parses a character as a value. -1 if the character is improper.
 *
 * @param       sptr                string pointer
 * @param       err_f               error file
 * @return                          parsed character value
 */
[[nodiscard]] int parse_char(strptr *const sptr, rprt_f *const err_f) {         // character to value parser
    const       msg_info    chr_err =   { .type=msg_err_t,  .header="character error",  .report_f=err_f };
    const       msg_info    chr_wrn =   { .type=msg_warn_t, .header="escape character", .report_f=err_f };

    int     ret;
    // escape character check
    if (*sptr->str == ESC_CHR) {
        inc_strptr(sptr);
        ret         =   esc_chr_val_(*sptr->str);
        if (ret == -1) {
            // report error
            err_f->col  =   sptr->col;
            err_f->len  =   1;
            cit10a_msg(&chr_wrn, "invalid escape character; parsing as a normal character");
            ret         =   (int)*sptr->str;
        }
    } else {
        // find char value
        ret         =   (int)*sptr->str;
    }

    // get ending character
    inc_strptr(sptr);
    if (*sptr->str != CHR_CHR) {
        // invalid end
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg(&chr_err, "invalid character end");
        return  -1;
    }

    inc_strptr(sptr);
    return  ret;
}

/**
 * Find a number (positive representation), and store the representation and value.
 *
 * @param       sptr            sptr
 * @param       err_f           error report file
 * @return                      number
 */
[[nodiscard]] int parse_num(strptr *const sptr, rprt_f *const err_f) {          // number parser
    cit10a_asrt(is_num_strt_ns(*sptr->str));

    // get numeric slice
    const   size_t  start   =   sptr->col;
    src_slice       slice   =   { .str=sptr->str, .len=0 };
    if (*sptr->str == CHR_CHR) {
        inc_strptr(sptr);
        return  parse_char(sptr, err_f);
    }

    if (*sptr->str == HEX_CHR_ALT)  inc_strptr(sptr);
    for (; is_alphanum(*sptr->str); inc_strptr(sptr), ++slice.len);

    // single number
    if (slice.len == 1)         return  (int)strtol(slice.str, nullptr, 10);

    // get parse start
    const   char   *str     =   slice.str;
    size_t          offs    =   0;

    // get base
    int             base    =   10;
    if (*slice.str == HEX_CHR_ALT) {
        base    =   16;
        str     +=  1;
        offs    +=  1;
    } else if (*slice.str == '0' && !('0' <= slice.str[1] && slice.str[1] <= '9') && slice.str[1] != NUM_SEP) {
        str         +=  2;
        offs        +=  2;
        switch (to_lwr_chr(slice.str[1])) {
            case 'x':
                base        =   16;
                break;
            case 'd':
                // default
                break;
            case 'o':
                base        =   8;
                break;
            case 'b':
                base        =   2;
                break;
            default:
                err_f->ln   =   sptr->ln;
                err_f->col  =   start + 1;
                err_f->len  =   1;
                cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid base specifier", .report_f=err_f },
                            "base specifier must be one of %c, 'x', 'd', 'o', or 'b'", HEX_CHR_ALT             );
                return  -1;
        }
    }

    // parse number
    char    n_str[MAX_PARSE + 1]        =   { '\0' };
    size_t  s_idx                       =   0;
    for (size_t i = 0; str[i] == NUM_SEP || is_alphanum(str[i]); ++i) {
        // skip number seperators
        if (str[i] != NUM_SEP)  n_str[s_idx++]  =   str[i];
        if (s_idx > MAX_PARSE) {
            err_f->ln   =   sptr->ln;
            err_f->col  =   start + offs;
            err_f->len  =   i;
            cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid number", .report_f=err_f },
                        "number too large to parse (maximally %d-bit)", MAX_PARSE                  );
            return  -1;
        }
    }
    if (s_idx == 0) {
        err_f->ln   =   sptr->ln;
        err_f->col  =   start + offs;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid number", .report_f=err_f },
                    "missing number", MAX_PARSE                                                );
        return  -1;
    }

    // convert number
    char   *end_chr;
    int     ret             =   (int)strtol(n_str, &end_chr, base);
    if (*end_chr != '\0') {
        // conversion fail
        err_f->ln   =   sptr->ln;
        err_f->col  =   start;
        err_f->len  =   sptr->col - start;
        cit10a_msg(&(msg_info){ .type=msg_err_t, .header="number parse error", .report_f=err_f}, "invalid number");
        ret         =   -1;
    }
    return  ret;
}

/**
 * Find a number's value, storing it as a two's compliment representation.
 *
 * @param       sptr            sptr
 * @param       err_f           error report file
 * @return                      number
 */
[[nodiscard]] int parse_num_repr(strptr *const sptr, rprt_f *const err_f) {     // number parser (negative repr)
    cit10a_asrt(sptr != nullptr && sptr->str != nullptr);
    cit10a_asrt(err_f != nullptr);

    // check negative
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    const   bool    neg         =   *sptr->str == NEG_SYMB;
    const   bool    pos         =   *sptr->str == POS_SYMB;
    if (neg || pos)                 inc_strptr(sptr);

    // get value
    if (!is_num_strt_ns(*sptr->str)) {
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="expected number", .report_f=err_f },
                    "invalid number start"                                                      );
        return  -1;
    }

    // parse number
    const   int p_strt      =   sptr->col;
    const   int const_v     =   parse_num(sptr, err_f);
    if (const_v == -1)          return  -1;

    int         max_num     =   MAX_NUM;
    if (neg)    max_num     =   MAX_NUM_NEG;
    if (pos)    max_num     =   MAX_NUM_POS;

    if (const_v > max_num) {
        err_f->col                  =   p_strt;
        err_f->len                  =   sptr->col - p_strt;
        const   char   *range_err   =   "";
        if (neg)        range_err   =   "negative ";
        if (pos)        range_err   =   "positive ";
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="number overflow", .report_f=err_f },
                    "maximum %snumber range excession", range_err                               );
        return  -1;
    }
    return  (neg) ? (const_v ^ (int)MAX_NUM) + 1 : const_v;
}

/**
 * Find the maximum address number.
 *
 * @param       sptr            sptr
 * @param       err_f           error report file
 * @return                      number
 */
[[nodiscard]] int parse_num_adrs(strptr *const sptr, rprt_f *const err_f) {     // address number parser
    cit10a_asrt(sptr != nullptr && sptr->str != nullptr);
    cit10a_asrt(err_f != nullptr);

    for (; is_whitespace(*sptr->str); inc_strptr(sptr));

    // get value
    if (!is_num_strt_ns(*sptr->str)) {
        err_f->col  =   sptr->col;
        err_f->len  =   1;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="expected number", .report_f=err_f },
                    "invalid number start"                                                      );
        return  -1;
    }

    // parse number
    const   int p_strt      =   sptr->col;
    const   int const_v     =   parse_num(sptr, err_f);
    if (const_v == -1)          return  -1;

    if (const_v > (int)MAX_ADRS) {
        err_f->ln                   =   sptr->ln;
        err_f->col                  =   p_strt;
        err_f->len                  =   sptr->col - p_strt;
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="number overflow", .report_f=err_f },
                    "maximum address range excession"                                           );
        return  -1;
    }
    return  const_v;
}
