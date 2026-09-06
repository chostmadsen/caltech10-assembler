/**
 * inc/segmenter/dataseg.h
 * documentation @ src/segmenter/dataseg.c
 */

#pragma     once

#include    "datastructures/stackmap.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {
            smap_head   head;
            size_t      loc;
} data_var;
