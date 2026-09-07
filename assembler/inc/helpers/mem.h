/**
 * inc/helpers/mem.h
 * documentation @ src/helpers/mem.c
 */

#pragma     once

#include    <stdlib.h>

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] void *chckd_malloc(size_t size, const char *itm);                 // checked malloc
[[nodiscard]] void *chckd_calloc(size_t n, size_t size, const char *itm);       // checked calloc
[[nodiscard]] void *chckd_realloc(void *ptr, size_t size, const char *itm);     // checked realloc

/*-MACROS-------------------------------------------------------------------------------------------------------------*/

/**
 * Prevents use-after-frees by setting pointer to nullptr on debug mode (expanded macro).
 * Use when the pointer is not stored on the stack - if the pointer does not exist outside the function scope,
 * setting the pointer to nullptr is pointless.
 *
 * @param       ptr             memory address to free
 */
#ifndef NDEBUG
#include    "output/errors.h"

#define safe_free(ptr) (                                                                                               \
            cit10a_asrt(ptr != nullptr),                                                                               \
            free(ptr),                                                                                                 \
            ptr     =   nullptr                                                                                        \
        )
#else
#define safe_free(ptr)          free(ptr)
#endif
