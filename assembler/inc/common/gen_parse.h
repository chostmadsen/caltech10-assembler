/**
 * inc/common/gen_parse.h
 * documentation @ src/common/gen_parse.c
 */

#pragma     once

#include    "output/messages.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   unsigned    MAX_NUM_PARSE       =   8;                              // maximum number parse (8-bit max)
constexpr   unsigned    MAX_NUM             =   (1 << MAX_NUM_PARSE) - 1;       // maximum number
constexpr   unsigned    MAX_NUM_NEG         =   1 << (MAX_NUM_PARSE - 1);       // maximum negative number (2s comp)
constexpr   unsigned    MAX_NUM_POS         =   (1 << (MAX_NUM_PARSE - 1)) - 1; // maximum positive number

constexpr   char        HEX_CHR_ALT         =   '$';                            // alternate hexadecimal character

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] bool check_ln_end(strptr *sptr, rprt_f *err_f);                   // line end checker

[[nodiscard]] int parse_num(strptr *sptr, rprt_f *err_f);                       // number parser
[[nodiscard]] int parse_num_repr(strptr *sptr, rprt_f *err_f);                  // number parser (negative repr)
