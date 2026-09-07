/**
 * perfhash/inc/hash_populate.h
 * documentation @ perfhash/src/hash_populate.c
 */

#ifndef PERFHASH_HASH_POPULATE_
#define PERFHASH_HASH_POPULATE_

#include    <stddef.h>
#include    <stdint.h>

#include    "hash_table.h"

/*-PERFECT-HASH-GROUP-OBJECT------------------------------------------------------------------------------------------*/

typedef struct {                                                                // hash group
    const   hash_itm      **arr;
            size_t          len;
            size_t          size;
} hash_grp;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] hash_grp new_hash_grp(const hash_itm *itm_arr, size_t size);      // hash group setup

[[maybe_unused]] void print_hash_grp(const hash_grp *group);                    // hash group printer
void free_hash_grp(hash_grp *group);                                            // hash group free

#endif  /* PERFHASH_HASH_POPULATE_ */
