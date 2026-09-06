/**
 * inc/helpers/general.h
 * documentation @ src/helpers/general.c
 */

#pragma     once

#include    <stddef.h>
#include    <stdio.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE                         128
#endif  /* CACHE_LINE_SIZE */
constexpr   unsigned    CACHE_LN_S          =   CACHE_LINE_SIZE;                // cache line size
_Static_assert(CACHE_LN_S > 0, "cache line size must be a positive integer");
_Static_assert(!(CACHE_LN_S & (CACHE_LN_S - 1)), "cache line size must be a power of two");

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // source slice (RO char*, no `\0`)
    const   char               *str;
            size_t              len;
} src_slice;

typedef struct {                                                                // string pointer to constant text
    const   char               *str;
            size_t              idx;
} strptr;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_src_slice(const src_slice *slice, FILE *stream);                     // source slice printer
[[nodiscard]] bool srcslc_eq(const src_slice *slc_1, const src_slice *slc_2);   // source slice equality check
[[nodiscard]] bool srcslc_eq_lwr( const src_slice *slc_1, 
                                  const src_slice *slc_2  );                    // source slice equality check (lower)

void adj_strptr(strptr *ptr, int n);                                            // string pointer adjust
void inc_strptr(strptr *ptr);                                                   // string pointer increment
void dec_strptr(strptr *ptr);                                                   // string pointer decrement
[[nodiscard]] char to_lwr_chr(char chr);                                        // character to lower

/*-MACROS-------------------------------------------------------------------------------------------------------------*/

/**
 * Wrapper to get array size.
 *
 * @param       arr             array
 */
#define arr_s(arr)          (sizeof(arr) / sizeof(arr[0]))
