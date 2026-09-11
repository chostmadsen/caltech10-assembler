/**
 * inc/segmenter/constseg.h
 * documentation @ src/segmenter/constseg.c
 */

#pragma     once

#include    <stddef.h>

#include    "datastructures/stackmap.h"
#include    "reader/reader.h"
#include    "preprocessor/folder_parse.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      CONST_BUCKETS       =   32;                             // initial const stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // constant var stackmap head
            var_tok     var;
            int         val;
} const_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_const_map(const stackmap *smap);                                     // constant stackmap printer
[[nodiscard]] bool constseg( const src_f    *source,
                             const sources  *srcs,
                                   stackmap *smap    );                         // constant stackmap creation
