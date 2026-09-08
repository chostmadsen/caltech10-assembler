/**
 * inc/segmenter/headerseg.h
 * documentation @ src/segmenter/headerseg.c
 */

#pragma     once

#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      HEADER_BUCKETS      =   64;                             // initial header stackmap buckets
constexpr   unsigned    LINE_INIT           =   256;                            // initial header stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // header stackmap head
            var_tok     var;
            int         loc;
} header_var;

typedef struct {                                                                // line information
            int         ln;
            int         loc;
} ln_info;

typedef struct {                                                                // header stackmap
            stack       stmts;
            stackmap    smap;
} headermap;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_header_map(const headermap *hmap);                                   // header stackmap printer
[[nodiscard]] bool headerseg(const src_f *source, headermap *hmap);             // header stackmap creation
