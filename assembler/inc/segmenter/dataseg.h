/**
 * inc/segmenter/dataseg.h
 * documentation @ src/segmenter/dataseg.c
 */

#pragma     once

#include    "datastructures/stackmap.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      DATA_BUCKETS        =   32;                             // initial data stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // data stackmap head
            var_tok     var;
            int         loc;
} data_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_data_map(const stackmap *smap);                                      // data stackmap printer
[[nodiscard]] stackmap dataseg(const src_f *source, bool *err);                 // data stackmap creation
