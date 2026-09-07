/**
 * perfhash/inc/file_emit.h
 * documentation @ perfhash/src/file_emit.c
 */

#ifndef PERFHASH_FILE_EMIT_
#define PERFHASH_FILE_EMIT_

#include    <stddef.h>
#include    <stdint.h>

#include    "hash_table.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#define                 HASH_TABLE_LOC          "perfhash/inc/hash_table.h"     // hash table location
constexpr   int         LINE_MAX            =   120;                            // maximum line size
constexpr   unsigned    TABSTOP             =   4;                              // tap stop size
constexpr   int         CMT_ALN             =   80;                             // comment alignment

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void hash_full( const hash_itm *itm_arr,
                      size_t    size,
                      uint64_t  seed,
                      size_t    off_init,
                      size_t    max_attempts,
                const char     *inc,
                const char     *name,
                const char     *file          );                                // perfect hashing full call

#endif  /* PERFHASH_FILE_EMIT_ */
