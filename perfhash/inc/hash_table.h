/**
 * perfhash/inc/hash_table.h
 * Hash table struct.
 */

#ifndef PERFHASH_HASH_TABLE_
#define PERFHASH_HASH_TABLE_

#include    <stddef.h>
#include    <stdint.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    FNV_PRIME_64_HT     =   0x00000100000001b3;             // fnv-1a 64-bit prime (hash table copy)

/*-PERFECT-HASH-CONSTRUCTION-OBJECT-----------------------------------------------------------------------------------*/

typedef struct {                                                                // perfect hash group
    const   char               *str;
    const   char        *const  instr_str;
    const   char        *const  tok_str,    *const  grp_str;
    const   bool                no_case;
} hash_itm;

typedef struct {                                                                // token item
            int                 tok;
            int                 grp;
            int                 instr;
} tok_itm;

typedef struct {                                                                // hashmap entry
    const   uint64_t            hash;
    const   char        *const  str;
    const   size_t              len;
    const   tok_itm             itm;
} hash_entry;

#endif  /* PERFHASH_HASH_TABLE_ */
