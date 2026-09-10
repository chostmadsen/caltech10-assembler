/**
 * inc/segmenter/dataseg.h
 * documentation @ src/segmenter/dataseg.c
 */

#pragma     once

#include    <stddef.h>
#include    <limits.h>

#include    "datastructures/stackmap.h"
#include    "common/gen_parse.h"
#include    "reader/reader.h"
#include    "segmenter/segment.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      DATA_BUCKETS        =   32;                             // initial data stackmap buckets

constexpr   int         D_FLD_SHFT          =   sizeof(size_t) * CHAR_BIT;
constexpr   int         N_D_FIELDS          =   (MAX_NUM + D_FLD_SHFT - 1)
                                                / D_FLD_SHFT;                   // bitfield number

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // data stackmap head
            var_tok     var;
            int         loc;
} data_var;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_data_map(const stackmap *smap);                                      // data stackmap printer
[[nodiscard]] bool dataseg(const src_f *source, stackmap *smap);                // data stackmap creation
