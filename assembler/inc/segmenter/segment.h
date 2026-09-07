/**
 * inc/segmenter/segment.h
 * documentation @ src/segmenter/segment.c
 */

#pragma     once

#include    "helpers/general.h"
#include    "output/messages.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#define                 DATA_SEG                "data"                          // data segment start
#define                 CODE_SEG                "code"                          // code segment start
#define                 ORG_SEG                 "org"                           // org specification

constexpr   char        PSEUDOOP_CHR        =   '.';                            // pseudo-op start character
constexpr   char        NUM_SEP             =   '_';                            // number seperator
constexpr   char        NEG_SYMB            =   '-';                            // negative value symbol

constexpr   unsigned    MAX_NUM_PARSE       =   8;                              // maximum number parse (8-bit max)
constexpr   unsigned    MAX_NUM             =   (1 << MAX_NUM_PARSE) - 1;       // maximum number
constexpr   unsigned    MAX_NUM_NEG         =   1 << (MAX_NUM_PARSE - 1);       // maximum negative number

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] int parse_num(rprt_f *err_f, strptr *text);                       // number parser
[[nodiscard]] int org_parse(rprt_f *err_f, strptr *text);                       // .org parser
