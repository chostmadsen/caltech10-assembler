/**
 * inc/segmenter/headerseg.h
 * documentation @ src/segmenter/headerseg.c
 */

#pragma     once

#include    "datastructures/stackmap.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      HEADER_BUCKETS      =   64;                             // initial header stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // header stackmap head
            var_tok     var;
            int         loc;
} header_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_header_map(const stackmap *smap);                                    // header stackmap printer
[[nodiscard]] stackmap headerseg(const src_f *source, bool *err);               // header stackmap creation
