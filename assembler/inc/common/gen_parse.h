/**
 * inc/common/gen_parse.h
 * documentation @ src/common/gen_parse.c
 */

#pragma     once

#include    "helpers/general.h"
#include    "output/messages.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   unsigned    MAX_NUM_PARSE       =   8;                              // maximum number parse (8-bit max)
constexpr   unsigned    MAX_NUM             =   (1 << MAX_NUM_PARSE) - 1;       // maximum number
constexpr   unsigned    MAX_NUM_NEG         =   1 << (MAX_NUM_PARSE - 1);       // maximum negative number (2s comp)
constexpr   unsigned    MAX_NUM_POS         =   (1 << (MAX_NUM_PARSE - 1)) - 1; // maximum positive number
constexpr   unsigned    MAX_ADRS_PARSE      =   13;                             // maximum address parse (13-bit max)
constexpr   unsigned    MAX_ADRS            =   (1 << MAX_ADRS_PARSE) - 1;      // maximum address
constexpr   unsigned    MAX_PARSE           =   (MAX_ADRS_PARSE > MAX_NUM_PARSE)
                                                ? MAX_ADRS_PARSE
                                                : MAX_NUM_PARSE;                // maximum number parser parse

constexpr   char        HEX_CHR_ALT         =   '$';                            // alternate hexadecimal character

constexpr   char        CHR_CHR             =   '\'';                           // character value
constexpr   char        ESC_CHR             =   '\\';                           // escape character
#define ESC_CHRS        /* chr, val */                                                                                 \
    X( 'a',     '\a' )                                                                                                 \
    X( 'b',     '\b' )                                                                                                 \
    X( 'f',     '\f' )                                                                                                 \
    X( 'n',     '\n' )                                                                                                 \
    X( 'r',     '\r' )                                                                                                 \
    X( 't',     '\t' )                                                                                                 \
    X( 'v',     '\v' )                                                                                                 \
    X( '\\',    '\\' )                                                                                                 \
    X( '\'',    '\'' )                                                                                                 \
    X( '"',     '\"' )                                                                                                 \
    X( '?',     '\?' )                                                                                                 \
    X( '0',     '\0' )

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] bool check_ln_end(strptr *sptr, rprt_f *err_f);                   // line end checker
[[nodiscard]] src_slice get_ident(strptr *sptr, rprt_f *err_f);                 // identifier parser

[[nodiscard]] bool is_num_strt(char chr);                                       // number start checker
[[nodiscard]] bool is_num_strt_ns(char chr);                                    // number start checker (non-signed)
[[nodiscard]] int parse_char(strptr *sptr, rprt_f *err_f);                      // character to value parser
[[nodiscard]] int parse_num(strptr *sptr, rprt_f *err_f);                       // number parser
[[nodiscard]] int parse_num_repr(strptr *sptr, rprt_f *err_f);                  // number parser (negative repr)
[[nodiscard]] int parse_num_adrs(strptr *sptr, rprt_f *err_f);                  // address number parser
