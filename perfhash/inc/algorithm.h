/**
 * perfhash/inc/algorithm.h
 * documentation @ perfhash/src/algorithm.c
 */

#ifndef PERFHASH_ALGORITHM_
#define PERFHASH_ALGORITHM_

#include    <stddef.h>
#include    <stdint.h>

#include    "hash_populate.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    FNV_PRIME_64        =   0x00000100000001b3;             // fnv-1a 64-bit prime

/*-PERFECT-HASH-CONSTRUCTION-OBJECT-----------------------------------------------------------------------------------*/

typedef struct {                                                                // flat string map
            char           *strs;
    const   size_t          size;
} str_map;

typedef struct {                                                                // bool bitset
            uint64_t    *const  mask;
    const   size_t              size;
} bitset;

typedef struct {                                                                // hash function constant struct
    const   uint64_t        offset;
    const   size_t          buckets;
} hash_fn;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] uint64_t hash_fn_impl(const char *str, uint64_t offset);          // hash function
[[nodiscard]] hash_fn algorithm( const hash_grp *group,
                                       uint64_t  seed,
                                       size_t    max_attempt,
                                       size_t    off_init     );                // perfect hash algorithm

#endif  /* PERFHASH_ALGORITHM_ */
