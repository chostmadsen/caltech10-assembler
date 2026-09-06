/*
 * src/segmenter/segment.c
 * Segmenter orchestrator.
 */

#include    <stddef.h>

#include    "helpers/general.h"
#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "segmenter/segment.h"

/*-SEGMENTER-HELPERS--------------------------------------------------------------------------------------------------*/

/**
 * Check if the given character is alphanumeric.
 *
 * @param       chr             character
 * @return                      if alphabetic
 */
[[nodiscard]] static bool is_alphanum_(const char chr) {                        // alphanumeric character
    return  ( ( 'A' <= chr && chr <= 'Z' ) || ( 'a' <= chr && chr <= 'z' ) ||
              ( '0' <= chr && chr <= '9' ) || ( '_' == chr               )    );
}

/*-NUMBER-PARSER------------------------------------------------------------------------------------------------------*/

/**
 * Find a number (positive representation), and store the representation and value.
 *
 * @param       err_f           error report file
 * @param       text            text
 * @return                      number
 */
[[nodiscard]] int parse_num(rprt_f *const err_f, strptr *const text) {          // number parser
    acama_asrt(('0' <= *text->str && *text->str <= '9') || *text->str == '$');

    // get numeric slice
    const   size_t  start   =   text->idx;
    src_slice       slice   =   { .str=text->str, .len=0 };
    for (; is_alphanum_(*text->str); inc_strptr(text), ++slice.len);

    // single number
    if (slice.len == 1)         return  (int)strtol(text->str, nullptr, 10);

    // get parse start
    const   char   *str     =   slice.str;
    size_t          m_size  =   slice.len + 1;

    // get base
    int             base    =   10;
    if (*slice.str == '$') {
        base    =   16;
    } else if (*slice.str == '0' && !('0' <= slice.str[1] && slice.str[1] <= '9') && slice.str[1] != NUM_SEP) {
        m_size      -=  2;
        str         +=  2;
        switch (to_lwr_chr(slice.str[1])) {
            case 'x':
                base    =   16;
                break;
            case 'd':
                // default
                break;
            case 'o':
                base    =   8;
                break;
            case 'b':
                base    =   2;
                break;
            default:
                find_file_loc(err_f, start + 1);
                acama_msg( &(msg_info){ .type=msg_err_t, .header="invalid base specifier", .report_f=err_f },
                           "base specifier must be one of '$', 'x', 'd', 'o', or 'b'"                         );
                return  -1;
        }
    }

    // parse number
    char    *const  n_str   =   chckd_malloc(m_size * sizeof(char), "string conversion char*");
    size_t          s_idx   =   0;
    for (size_t i = 0; i < m_size - 1; ++i) {
        // skip number seperators
        if (str[i] != NUM_SEP)  n_str[s_idx++]  =   str[i];
    }
    n_str[s_idx]            =   '\0';

    // convert number
    char   *end_chr;
    int     ret             =   (int)strtol(n_str, &end_chr, base);
    if (*end_chr != '\0') {
        // conversion fail
        find_file_loc(err_f, start);
        err_f->len  =   text->idx - start;
        acama_msg(&(msg_info){ .type=msg_err_t, .header="number parse error", .report_f=err_f}, "invalid number");
        ret         =   -1;
    }
    free(n_str);
    return  ret;
}

/*-.ORG-PARSER--------------------------------------------------------------------------------------------------------*/

/**
 * Try to return item after a .org specification.
 *
 * @param       err_f           error report file
 * @param       text            text
 * @return                      number
 */
[[nodiscard]] int org_parse(rprt_f *const err_f, strptr *const text) {          // .org parser
    // find context
    while (is_whitespace(*text->str))   inc_strptr(text);

    // try to parse number
    if (('0' <= *text->str && *text->str <= '9') || *text->str == '$')      return  parse_num(err_f, text);
    find_file_loc(err_f, text->idx);
    acama_msg( &(msg_info){ .type=msg_err_t, .header="expected number", .report_f=err_f},
               "expected a number after `.org`"                                           );
    return  -1;
}
