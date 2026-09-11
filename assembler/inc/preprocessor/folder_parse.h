/**
 * inc/preprocessor/folder_parse.h
 * documentation @ src/preprocessor/folder_parse.
 */

#pragma     once

#include    <stddef.h>

#include    "datastructures/stackmap.h"
#include    "output/messages.h"
#include    "reader/reader.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   char        FOLD_SEP            =   '/';                            // folder seperator
constexpr   size_t      SRC_F_SMAP_BCKTS    =   32;                             // initial folder stackmap buckets
constexpr   size_t      FMAP_SHFT           =   1;                              // file stackmap overshift

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // source file filename item
            smap_head   head;
    const   char       *folder;
} fname_itm;

typedef struct {                                                                // source file group item
            smap_head   head;
            src_f      *source;
} src_f_sm;

typedef struct {                                                                // source file group
            bool        empty;
            stackmap    open_files;  /* src_f_sm */
            stackmap    files;       /* smap_head */
            char        flag;
} src_grp;

typedef struct {                                                                // assembly sources
            src_grp    *items;
            size_t      num;
} sources;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] sources get_sources(const size_t count, ...);                     // get sources for flags

[[nodiscard]] src_f *get_source( const src_slice *f_name,
                                       sources   *srcs,
                                 const rprt_f    *err_f   );                    // source file getter
[[nodiscard]] src_f *get_source_static( const src_slice *f_name,
                                        const sources   *srcs    );             // static source file getter

void free_sources(sources *srcs);                                               // frees source files
