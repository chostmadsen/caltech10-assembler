/**
 * src/helpers/mem.c
 * Safer memory management functions.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <errno.h>

#include    "helpers/general.h"
#include    "output/messages.h"
#include    "output/errors.h"

/*-ALLOCATION-FUNCTIONS-----------------------------------------------------------------------------------------------*/

static  const       msg_info    alloc_msg   =   { .type=msg_intrnl_t, .header="alloc error" };

/**
 * Mallocs the given size, then checks if the allocation was sucessful.
 * If not, exits with the given item description in the message.
 *
 * @param       size            alloc size
 * @param       itm             item description
 * @return                      allocated memory pointer
 */
[[nodiscard]] void *chckd_malloc(const size_t size, const char *const itm) {    // checked malloc
    cit10a_asrt(size != 0);

    errno                   =   0;
    void    *const  ret     =   malloc(size);
    if (ret == nullptr) {
        // alloc failure
        cit10a_msg(&alloc_msg, "%s malloc [ %zuB - errno %d ]", itm, size, errno);
        cit10a_exit(ALLOC_ERRNO);
    }
    return  ret;
}

/**
 * Callocs the given size, then checks if the allocation was sucessful.
 * If not, exits with the given item description in the message.
 *
 * @param       size            alloc element size
 * @param       n               number of items
 * @param       itm             item description
 * @return                      allocated memory pointer
 */
[[nodiscard]] void *chckd_calloc( const size_t        n,
                                  const size_t        size, 
                                  const char   *const itm   ) {                 // checked calloc
    cit10a_asrt(n != 0 && size != 0);

    errno                   =   0;
    void    *const  ret     =   calloc(n, size);
    if (ret == nullptr) {
        // alloc failure
        cit10a_msg(&alloc_msg, "%s calloc [ %zuB - errno %d ]", itm, n * size, errno);
        cit10a_exit(ALLOC_ERRNO);
    }
    return  ret;
}

/**
 * Reallocs the given size, then checks if the allocation was sucessful.
 * If not, exits with the given item description in the message.
 *
 * @param       ptr             previous pointer
 * @param       size            alloc size
 * @param       itm             item description
 * @return                      allocated memory pointer
 */
[[nodiscard]] void *chckd_realloc(       void   *const ptr,
                                   const size_t        size,
                                   const char   *const itm   ) {                // checked realloc
    cit10a_asrt(size != 0);

    errno                   =   0;
    void    *const  ret     =   realloc(ptr, size);
    if (ret == nullptr) {
        // alloc failure
        cit10a_msg(&alloc_msg, "%s realloc [ %zuB - errno %d ]", itm, size, errno);
        cit10a_exit(ALLOC_ERRNO);
    }
    return  ret;
}

/**
 * Aligned allocation for the given size, then check sif the allocation was successful.
 * If not, exits with the given item description in the message.
 *
 * @param       size            alloc size
 * @param       itm             item description
 * @return                      allocated memory pointer
 */
[[nodiscard]] void *chckd_aln_alloc(const size_t size, const char *const itm) { // checked aligned alloc
    cit10a_asrt(size != 0);

    const   size_t  pad     =   (size + (size_t)CACHE_LN_S - 1) & ~((size_t)CACHE_LN_S - 1);
    errno                   =   0;
    void    *const  ret     =   aligned_alloc(CACHE_LN_S, pad);
    if (ret == nullptr) {
        // alloc failure
        cit10a_msg(&alloc_msg, "%s aligned alloc [ %zuB - errno %d ]", itm, pad, errno);
        cit10a_exit(ALLOC_ERRNO);
    }
    return  ret;
}
