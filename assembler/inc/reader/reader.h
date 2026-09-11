/**
 * inc/reader/reader.h
 * documentation @ src/reader/reader.c
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      F_PAD               =   128;                            // number of end padding bytes

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // source file struct
            char       *f_name;
            char       *text;
            size_t     *ln_idxs;
            size_t      ln_num,     len,    size;
} src_f;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] src_f read_source(const char *f_name);                            // read source file

[[nodiscard]] const char *src_f_getline(const src_f *source_f, size_t idx);     // readline of struct
void free_src_f(src_f *source_f);                                               // free source struct

[[nodiscard]] bool newln_strptr(strptr *sptr, const src_f *source);             // strptr newline
void setln_strptr(strptr *sptr, const src_f *source, size_t ln);                // strptr set line
