/**
 * inc/segmenter/constseg.h
 * documentation @ src/segmenter/constseg.c
 */

#pragma     once

#include    "datastructures/stackmap.h"
#include    "reader/reader.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      CONST_BUCKETS       =   8;                              // initial const stackmap buckets

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // constant var stackmap head
            smap_head   head;
            int         val;
} const_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] stackmap constseg(const src_f *source, bool *err);                // constant stackmap creation
void print_const_map(const stackmap *smap);                                     // constant stackmap printer
