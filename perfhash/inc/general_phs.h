/**
 * perfhash/inc/general_phs.h
 * documentation @ perfhash/src/general_phs.c
 */

#ifndef PERFHASH_GENERAL_PHS_
#define PERFHASH_GENERAL_PHS_

#include    <stddef.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE                         128
#endif  /* CACHE_LINE_SIZE */
constexpr   unsigned    CACHE_LN_S          =   CACHE_LINE_SIZE;                // cache line size
_Static_assert(CACHE_LN_S > 0, "cache line size must be a positive integer");
_Static_assert(!(CACHE_LN_S & (CACHE_LN_S - 1)), "cache line size must be a power of two");

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] void *s_malloc(size_t size);                                      // safe malloc
[[nodiscard]] void *s_calloc(size_t n, size_t size);                            // safe calloc
[[nodiscard]] void *s_realloc(void *ptr, size_t size);                          // safe realloc
[[nodiscard]] void *s_aln_alloc(size_t size);                                   // safe aligned alloc

/*-MACROS-------------------------------------------------------------------------------------------------------------*/

/**
 * Wrapper to get array size.
 *
 * @param       arr             array
 */
#define arr_s(arr)          (sizeof(arr) / sizeof(arr[0]))

#endif  /* PERFHASH_GENERAL_PHS_ */
