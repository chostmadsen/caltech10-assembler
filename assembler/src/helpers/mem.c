/**
 * src/helpers/mem.c
 * Safer memory management functions.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <errno.h>

#include    "output/messages.h"
#include    "output/errors.h"

/*-ALLOCATION-FUNCTIONS-----------------------------------------------------------------------------------------------*/

static  const       msg_info    alloc_msg   =   { .type=msg_intrnl_t, .header="alloc error" };

/**
 * mallocs the given size, then checks if the allocation was sucessful.
 * If not, exits with the given item description in the message.
 *
 * @param       size            alloc size
 * @param       itm             item description
 * @return                      allocated memory pointer
 */
[[nodiscard]] void *chckd_malloc(const size_t size, const char *const itm) {    // checked malloc
    errno                   =   0;
    void    *const  ret     =   malloc(size);
    if (ret == nullptr) {
        // alloc failure
        acama_msg(&alloc_msg, "%s malloc [ %zuB - errno %d ]", itm, size, errno);
        acama_exit(ALLOC_ERRNO);
    }
    return  ret;
}

/**
 * callocs the given size, then checks if the allocation was sucessful.
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
    errno                   =   0;
    void    *const  ret     =   calloc(n, size);
    if (ret == nullptr) {
        // alloc failure
        acama_msg(&alloc_msg, "%s calloc [ %zuB - errno %d ]", itm, n * size, errno);
        acama_exit(ALLOC_ERRNO);
    }
    return  ret;
}

/**
 * reallocs the given size, then checks if the allocation was sucessful.
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
    errno                   =   0;
    void    *const  ret     =   realloc(ptr, size);
    if (ret == nullptr) {
        // alloc failure
        acama_msg(&alloc_msg, "%s realloc [ %zuB - errno %d ]", itm, size, errno);
        acama_exit(ALLOC_ERRNO);
    }
    return  ret;
}
