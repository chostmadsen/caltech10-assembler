/**
 * inc/preprocessor/folder_parse.h
 * documentation @ src/preprocessor/folder_parse.
 */

#pragma     once

#include    <stddef.h>

#include    "reader/reader.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // source file group
            src_f      *sources;
            size_t      num
} src_f_grp;
