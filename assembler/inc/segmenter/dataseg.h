/**
 * inc/segmenter/dataseg.h
 * documentation @ src/segmenter/dataseg.c
 */

#pragma     once

#include    "datastructures/stackmap.h"
#include    "reader/reader.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      DATA_BUCKETS        =   8;                              // initial data stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // data stackmap head
            smap_head   head;
            int         loc;
} data_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] stackmap dataseg(const src_f *source, bool *err);                 // data stackmap creation
