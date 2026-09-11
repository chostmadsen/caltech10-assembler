/**
 * inc/segmenter/seg_orch.h
 * documentation @ src/segmenter/seg_orch.c
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"
#include    "datastructures/stackmap.h"
#include    "reader/reader.h"
#include    "preprocessor/folder_parse.h"
#include    "segmenter/headerseg.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      SEGMENT_NUM         =   3;                              // number of segment calls

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {
    alignas(CACHE_LN_S) stackmap   constmap;
    alignas(CACHE_LN_S) stackmap   datamap;
    alignas(CACHE_LN_S) headermap  headmap;
} segmaps;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] segmaps segment(const src_f *source, const sources *const srcs);  // segment orchestrator

void print_segmap(const segmaps *segmap);                                       // print segmap struct
void print_segmap_info(const segmaps *segmap);                                  // print segmap struct info

void free_segmap(segmaps *segmap);                                              // segmap free
