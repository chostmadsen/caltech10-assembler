/**
 * inc/common/hash_tables/pseudo.h
 * AUTO-GENERATED - DO NOT MANUALLY MODIFY; see perfhash/README.md for usage.
 */

#ifndef PSEUDO_HASH_TABLE_
#define PSEUDO_HASH_TABLE_

#include    <stddef.h>
#include    <string.h>
#include    <stdint.h>

#include    "perfhash/inc/hash_table.h"
#include    "common/kwrds.h"

/*-PSEUDO-CONSTANTS---------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    PSEUDO_OFFSET       =   0xf893a2eefb32555e;             // pseudo hash offset
constexpr   size_t      PSEUDO_TBL_S        =   8;                              // pseudo hash table size

constexpr   size_t      PSEUDO_MAX_STR      =   7;                              // pseudo hash table max string length

/*-PSEUDO-HASH-TABLE--------------------------------------------------------------------------------------------------*/

const   hash_entry  pseudo_table[PSEUDO_TBL_S]  =   {                           // pseudo hash table
    [2]     =   { 0x6fd6d7b4540bf04a, "data",    4, { .tok=tok_data,  .grp=pseudo_segspec_t, .instr=0 } },
    [7]     =   { 0x2406a9cc2754cf2f, "code",    4, { .tok=tok_code,  .grp=pseudo_segspec_t, .instr=0 } },
    [1]     =   { 0x3be2a69d65cafeb1, "org",     3, { .tok=tok_org,   .grp=pseudo_pseudoa_t, .instr=0 } },
    [5]     =   { 0x3d9618eb4724ec95, "include", 7, { .tok=tok_incl,  .grp=pseudo_pseudoa_t, .instr=0 } },
    [4]     =   { 0x56d726a9de56acdc, "const",   5, { .tok=tok_const, .grp=pseudo_pseudoa_t, .instr=0 } }
};

/*-PSEUDO-HASH-FUNCTION-----------------------------------------------------------------------------------------------*/

/**
 * Generates the hash for the pseudo hash table based on a string and number of characters, lowercased.
 *
 * @param       str             string to hash
 * @param       n               character number
 * @return                      hash
 */
[[nodiscard]] static uint64_t pseudo_hash_fn(const char *str, size_t n) {       // pseudo hash function
    uint64_t    hash    =   PSEUDO_OFFSET;
    while (n--) {
        const   char    chr =   *(str++);
        hash                ^=  chr | (uint8_t)((chr - 'A') < 26) << 5;
        hash                *=  FNV_PRIME_64_HT;
    }
    return  hash ^ (hash >> 32);
}

/*-PSEUDO-HASH-TABLE-LOOKUP-FUNCTION----------------------------------------------------------------------------------*/

/**
 * Performs a lookup in the pseudo hash table based on the provided string and hash (case insensitive).
 * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)
 *
 * @param       str             lookup string
 * @param       n               character number
 * @param       hash            string hash
 * @return                      table entry
 */
[[nodiscard]] static tok_itm pseudo_hash_lu_h( const char     *const str,
                                               const size_t          n,
                                               const uint64_t        hash ) {   // pseudo hash table lookup (w/ hash)
    // get table entry
    const   hash_entry  table_entry =   pseudo_table[hash & (PSEUDO_TBL_S - 1)];

    // check entry
    if (n != table_entry.len)                           return  (tok_itm){ .tok=0, .grp=0 };
    if (hash != table_entry.hash)                       return  (tok_itm){ .tok=0, .grp=0 };
    for(unsigned i = 0; i < table_entry.len; ++i) {
        const   char    chr_t   =   table_entry.str[i] | (uint8_t)((table_entry.str[i] - 'A') < 26) << 5;
        const   char    chr_l   =   str[i] | (uint8_t)((str[i] - 'A') < 26) << 5;
        if (chr_t != chr_l)         return  (tok_itm){ .tok=0, .grp=0 };
    }

    // return matched item
    return  table_entry.itm;
}

/**
 * Performs a lookup in the pseudo hash table based on the provided string (case insensitive).
 * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)
 *
 * @param       str             lookup string
 * @param       n               character number
 * @return                      table entry
 */
[[maybe_unused]] [[nodiscard]]
static tok_itm pseudo_hash_lu(const char *const str, const size_t n) {          // pseudo hash table lookup
    // generate hash and lookup
    return  pseudo_hash_lu_h(str, n, pseudo_hash_fn(str, n));
}

#endif  /* PSEUDO_HASH_TABLE_ */
