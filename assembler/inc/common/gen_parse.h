/**
 * inc/common/gen_parse.h
 * documentation @ src/common/gen_parse.c
 */

#pragma     once

#include    "helpers/general.h"
#include    "output/messages.h"
#include    "perfhash/inc/hash_table.h"
#include    "preprocessor/folder_parse.h"

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
constexpr   char        STR_CHR             =   '\"';                           // string value
constexpr   char        ESC_CHR             =   '\\';                           // escape character
constexpr   int         STR_CHR_LEN         =   1;                              // string character length
constexpr   int         ESC_CHR_LEN         =   1;                              // escape character length
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
// NOTE : update this in folder parsers too if you update this here
#define STR_ESC_CHRS    /* chr, val */                                                                                 \
    X( '\\',    '\\' )                                                                                                 \
    X( '"',     '\"' )

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] bool check_ln_end(strptr *sptr, rprt_f *err_f);                   // line end checker
[[nodiscard]] src_slice get_ident(strptr *sptr, rprt_f *err_f);                 // identifier parser
[[nodiscard]] src_f *get_inc_static(strptr *sptr, const sources *srcs);         // static inclusion get
[[nodiscard]] tok_itm pseudo_hash_lu_adj_tok(strptr *sptr);                     // .psuedo lookup w/ strptr adj
[[nodiscard]] int pseudo_hash_lu_adj(strptr *sptr);                             // .psuedo lookup w/ strptr adj

[[nodiscard]] bool is_num_strt(char chr);                                       // number start checker
[[nodiscard]] bool is_num_strt_ns(char chr);                                    // number start checker (non-signed)
[[nodiscard]] int parse_char(strptr *sptr, rprt_f *err_f);                      // character to value parser
[[nodiscard]] src_slice parse_str(rprt_f *err_f, strptr *text, bool *err);      // string value parser
[[nodiscard]] int parse_num(strptr *sptr, rprt_f *err_f);                       // number parser
[[nodiscard]] int parse_num_repr(strptr *sptr, rprt_f *err_f);                  // number parser (negative repr)
[[nodiscard]] int parse_num_adrs(strptr *sptr, rprt_f *err_f);                  // address number parser
