/**
 * perfhash/src/general_phs.c
 * General perfhash items.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <errno.h>

#include    "general_phs.h"

/*-MEMORY-ALLOCATION-HELPERS------------------------------------------------------------------------------------------*/

/**
 * `malloc` wrapper to report OOM errors.
 *
 * @param       size            malloc size
 * @return                      data pointer
 */
[[nodiscard]] void *s_malloc(const size_t size) {                               // safe malloc
    errno           =   0;
    void    *ret    =   malloc(size);
    if (ret == nullptr) {
        fprintf(stderr, "OOM [ errno %d ]\n", errno);
        exit(1);
    }
    return  ret;
}

/**
 * `calloc` wrapper to report OOM errors.
 *
 * @param       size            calloc number
 * @param       n               item size
 * @return                      data pointer
 */
[[nodiscard]] void *s_calloc(const size_t n, const size_t size) {               // safe calloc
    errno           =   0;
    void    *ret    =   (void*)calloc(n, size);
    if (ret == nullptr) {
        fprintf(stderr, "OOM [ errno %d ]\n", errno);
        exit(1);
    }
    return  ret;
}

/**
 * `realloc` wrapper to report OOM errors.
 *
 * @param       ptr             original data pointer
 * @param       size            malloc size
 * @return                      new data pointer
 */
[[nodiscard]] void *s_realloc(void *const ptr, const size_t size) {             // safe realloc
    errno           =   0;
    void    *ret    =   realloc(ptr, size);
    if (ret == nullptr) {
        fprintf(stderr, "OOM [ errno %d ]\n", errno);
        exit(1);
    }
    return  ret;
}
