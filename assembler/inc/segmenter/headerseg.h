/**
 * inc/segmenter/headerseg.h
 * documentation @ src/segmenter/headerseg.c
 */

#pragma     once

#include    <stddef.h>
#include    <limits.h>

#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "common/gen_parse.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      HEADER_BUCKETS      =   64;                             // initial header stackmap buckets
constexpr   unsigned    LINE_INIT           =   256;                            // initial header stackmap buckets

constexpr   int         H_FLD_SHFT          =   sizeof(size_t) * CHAR_BIT;
constexpr   int         N_H_FIELDS          =   (MAX_ADRS + H_FLD_SHFT - 1)
                                                / H_FLD_SHFT;                   // bitfield number

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // header stackmap head
            var_tok     var;
            int         loc;
} header_var;

typedef struct {                                                                // line information
            int         loc;
    const   src_f      *source;
            int         ln;
} ln_info;

typedef struct {                                                                // header stackmap
            stack       stmts;
            stackmap    smap;
} headermap;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_header_map(const headermap *hmap);                                   // header stackmap printer
[[nodiscard]] bool headerseg(const src_f *source, headermap *hmap);             // header stackmap creation
