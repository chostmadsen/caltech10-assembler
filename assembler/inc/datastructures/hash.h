/**
 * inc/datastructures/hash.h
 * documentation @ src/datastructures/hash.c
 */

#pragma     once

#include    <stddef.h>
#include    <stdint.h>

#include    "helpers/general.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    FNV_OFFSET_64       =   0xcbf29ce484222325;             // fnv-1a 64-bit offset
constexpr   uint64_t    FNV_PRIME_64        =   0x00000100000001b3;             // fnv-1a 64-bit prime

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] uint64_t hash_fnv1a(const char *str, size_t n);                   // 64-bit fnv1a hash
[[nodiscard]] uint64_t hash_fnv1a_slc(const src_slice *slice);                  // 64-bit fnv1a hash (slc)

[[nodiscard]] uint64_t hash_fnv1a_lwr(const char *str, size_t n);               // lower 64-bit fnv1a hash
[[nodiscard]] uint64_t hash_fnv1a_slc_lwr(const src_slice *slice);              // lower 64-bit fnv1a hash (slc)

[[nodiscard]] uint64_t hash_fnv1a_lwr_f(const char *str, size_t n, bool *flg);  // flagged lower 64-bit fnv1a hash
[[nodiscard]] uint64_t hash_fnv1a_slc_lwr_f(const src_slice *slice, bool *flg); // flagged lower 64-bit fnv1a hash (slc)
