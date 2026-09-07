/**
 * perfhash/inc/general_phs.h
 * documentation @ perfhash/src/general_phs.c
 */

#ifndef PERFHASH_GENERAL_PHS_
#define PERFHASH_GENERAL_PHS_

#include    <stddef.h>

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] void *s_malloc(size_t size);                                      // safe malloc
[[nodiscard]] void *s_calloc(size_t n, size_t size);                            // safe calloc
[[nodiscard]] void *s_realloc(void *ptr, size_t size);                          // safe realloc

/*-MACROS-------------------------------------------------------------------------------------------------------------*/

/**
 * Wrapper to get array size.
 *
 * @param       arr             array
 */
#define arr_s(arr)          (sizeof(arr) / sizeof(arr[0]))

#endif  /* PERFHASH_GENERAL_PHS_ */
