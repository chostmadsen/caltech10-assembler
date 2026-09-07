/**
 * src/datastructures/vstack.c
 * Value stack implementation functions.
 */

#include    <stddef.h>
#include    <stdlib.h>

#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "datastructures/vstack.h"

/*-INTERNAL-VSTACK-FUNCTIONS------------------------------------------------------------------------------------------*/

/**
 * Allocates vstack data on the heap. Caller is responsible for managing allocated memory.
 *
 * @param       size            initial vstack size
 * @return                      size_t* to vstack data
 */
[[nodiscard]] static size_t *alloc_vst_data_(const unsigned size) {             // vstack data allocation
    cit10a_asrt(size > 0);

    // data allocation
    size_t *vst_data    =   chckd_malloc(sizeof(size_t) * (int)size, "vstack array void*");
    return  vst_data;
}

/*-VSTACK-CREATION-FUNCTIONS------------------------------------------------------------------------------------------*/

// Implementations are called through macros defined in the corresponding .h file; see inc/helpers/vstack.h for reference.

/**
 * Creates a new vstack with the header on the stack. Freeing should be done by the `free_vstack` function.
 *
 * @param       size            initial vstack size
 * @return                      new vstack
 */
[[nodiscard]] vstack new_vstack(const unsigned size) {                          // vstack creation on the stack
    // vstack data allocation
    return  (vstack){ .data=alloc_vst_data_(size), .len=0, .size=(int)size };
}

/**
 * Creates a new vstack with the header on the heap. Freeing should be done by the `free_m_vstack` function.
 *
 * @param       size            initial vstack size
 * @return                      new vstack
 */
[[nodiscard]] vstack *malloc_vstack(const unsigned size) {                      // vstack creation on the heap
    // vstack header on heap
    vstack  *const  vst     =   chckd_malloc(sizeof(vstack), "vstack vstack*");

    // vstack data allocation
    *vst    =   new_vstack(size);
    return  vst;
}

/*-VSTACK-DATA-MODIFIER-/-VIEWER-FUNCTIONS----------------------------------------------------------------------------*/

/**
 * Pushes a value to the vstack.
 *
 * @param       vst             pointer to vstack
 * @param       val             value to push
 */
void push_vstack(vstack *const vst, const size_t val) {                         // push to vstack
    cit10a_asrt(vst != nullptr);

    if (++vst->len > vst->size) {
        // double size
        vst->size   *=  2;

        // realloc vstack data
        size_t  *const  new_data    =   chckd_realloc(vst->data, vst->size * sizeof(size_t), "vstack push void*");
        vst->data   =   new_data;
    }

    // push item
    vst->data[vst->len - 1] =   val;
}

/**
 * Returns the top value of the vstack, and decrements the vstack length.
 *
 * @param       vst             vstack
 * @return                      void* of popped data
 */
size_t pop_vstack(vstack *const vst) {                                          // pop from vstack
    cit10a_asrt(vst != nullptr && vst->len - 1 >= 0);
    return  vst->data[--vst->len];
}

/**
 * Removes 1 item from the top of the vstack.
 *
 * @param       vst             pointer to vstack
 */
void rm_vstack(vstack *const vst) {                                             // remove from vstack
    cit10a_asrt(vst != nullptr && vst->len - 1 >= 0);
    --vst->len;
}

/**
 * Removes n items from the top of the vstack.
 *
 * @param       vst             pointer to vstack
 * @param       n               number of items to remove
 */
void rm_vstack_n(vstack *const vst, const unsigned n) {                         // multiple removals from vstack
    cit10a_asrt(vst != nullptr && vst->len - (int)n >= 0);
    vst->len -=  (int)n;
}

/**
 * Peeks the vstack at item n.
 *
 * @param       vst             pointer to vstack
 * @param       n               peek index
 * @return                      vstack value
 */
[[nodiscard]] size_t peek_vstack(const vstack *const vst, const unsigned n) {   // vstack peek
    cit10a_asrt(vst != nullptr && vst->len > (int)n);
    return  vst->data[n];
}

/**
 * Peeks the vstack at the tail.
 *
 * @param       vst             pointer to vstack
 * @param       n               peek index
 * @return                      vstack value
 */
[[nodiscard]] size_t peek_vstack_tail(const vstack *const vst) {                // vstack peek at tail
    cit10a_asrt(vst->len != 0);
    return  vst->data[vst->len - 1];
}

/*-VSTACK-FREE-FUNCTIONS----------------------------------------------------------------------------------------------*/

/**
 * Frees the vstack's data; use `free_m_vstack` to free a heap allocated vstack.
 *
 * @param       vst          pointer to vstack
 */
void free_vstack(vstack *const vst) {                                           // free stack vstack
    cit10a_asrt(vst != nullptr);
    safe_free(vst->data);

#ifndef NDEBUG
    // struct zero
    vst->len    =   0;
    vst->size   =   0;
#endif  /* NDEBUG */
}

/**
 * Frees a vstack on the heap. Calls `free_vstack` to free the vstack's data, then frees the full vstack struct.
 *
 * @param       vst         pointer to pointer to vstack
 */
void free_m_vstack(vstack **const vst) {                                        // free heap vstack
    free_vstack(*vst);
    safe_free(*vst);
}
