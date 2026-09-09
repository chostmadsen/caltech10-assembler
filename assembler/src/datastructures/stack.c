/**
 * src/datastructures/stack.c
 * Stack implementation functions.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <string.h>

#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "datastructures/stack.h"

/*-INTERNAL-STACK-FUNCTIONS-------------------------------------------------------------------------------------------*/

/**
 * Allocates stack data w/ the specified size on the heap. Caller is responsible for managing allocated memory.
 *
 * @param       data_s          data size
 * @param       size            initial stack size
 * @param       aln             align allocation
 * @return                      void* to stack data
 */
[[nodiscard]] static void *alloc_st_data_( const size_t   data_s,
                                           const unsigned size,
                                           const bool     aln     ) {           // stack data allocation
    cit10a_asrt(data_s > 0 && size > 0);

    // data allocation
    void   *st_data     =   (aln) ? chckd_aln_alloc(data_s * (int)size, "algined stack array void*")
                                  : chckd_malloc(data_s * (int)size, "stack array void*");
    return  st_data;
}

/*-STACK-CREATION-FUNCTIONS-------------------------------------------------------------------------------------------*/

// Implementations are called through macros defined in the corresponding .h file; see inc/helpers/stack.h for reference.

/**
 * Creates a new stack with the header on the stack. Freeing should be done by the `free_stack` function.
 *
 * @param       data_s          data size
 * @param       size            initial stack size
 * @return                      new stack
 */
[[nodiscard]] stack new_stack_(const size_t data_s, const unsigned size) {      // stack creation on the stack
    // stack data allocation
    cit10a_asrt(size != 0);
    return  (stack){ .data=alloc_st_data_(data_s, size, false), .len=0, .size=(int)size, .data_s=data_s,
                     .aligned=false                                                                      };
}

/**
 * Creates a new stack with the header on the heap. Freeing should be done by the `free_m_stack` function.
 *
 * @param       data_s          data size
 * @param       size            initial stack size
 * @return                      new stack
 */
[[nodiscard]] stack *malloc_stack_(const size_t data_s, const unsigned size) {  // stack creation on the heap
    // stack header on heap
    stack   *const  st      =   chckd_malloc(sizeof(stack), "stack stack*");

    // stack data allocation
    *st     =   new_stack_(data_s, size);
    return  st;
}

/**
 * Creates a new aligned stack with the header on the stack. Freeing should be done by the `free_stack` function.
 *
 * @param       data_s          data size
 * @param       size            initial stack size
 * @return                      new stack
 */
[[nodiscard]] stack new_stack_aln(const size_t data_s, const unsigned size) {   // aligned stack creation on the stack
    // stack data allocation
    cit10a_asrt(size != 0);
    return  (stack){ .data=alloc_st_data_(data_s, size, true), .len=0, .size=(int)size, .data_s=data_s,
                     .aligned=true                                                                      };
}

/*-STACK-DATA-MODIFIER-/-VIEWER-FUNCTIONS-----------------------------------------------------------------------------*/

/**
 * Uses memcpy to push an item onto the given stack.
 *
 * @param       st              pointer to stack
 * @param       itm             pointer to item to push
 */
void push_stack(stack *const st, const void *const itm) {                       // push to stack
    cit10a_asrt(st != nullptr && itm != nullptr);

    if (++st->len > st->size) {
        // double size
        st->size    *=  2;

        // realloc stack data
        if (!st->aligned)       st->data    =   chckd_realloc(st->data, st->data_s * st->size, "stack push void*");
        else {
            void    *const  new_data        =   chckd_aln_alloc(st->data_s * st->size, "aligned stack push void*");
            memcpy(new_data, st->data, st->data_s * (size_t)(st->len - 1));
            free(st->data);
            st->data                        =   new_data;
        }
    }

    // push item
    memcpy((char*)st->data + ((st->len - 1) * st->data_s), itm, st->data_s);
}

/**
 * Returns a pointer to the top item of the stack, and identifies the popped item to be overwritten on the next stack
 * push operation. The returned pointer should be written to an external buffer if the popped item must be retained.
 *
 * @param       st              stack
 * @return                      void* of popped data
 */
void *pop_stack(stack *const st) {                                              // pop from stack
    cit10a_asrt(st != nullptr && st->len - 1 >= 0);
    return  (char*)st->data + (--st->len) * st->data_s;
}

/**
 * Removes 1 item from the top of the stack.
 *
 * @param       st              pointer to stack
 */
void rm_stack(stack *const st) {                                                // remove from stack
    cit10a_asrt(st != nullptr && st->len - 1 >= 0);
    --st->len;
}

/**
 * Removes n items from the top of the stack.
 *
 * @param       st              pointer to stack
 * @param       n               number of items to remove
 */
void rm_stack_n(stack *const st, const unsigned n) {                            // multiple removals from stack
    cit10a_asrt(st != nullptr && st->len - (int)n >= 0);
    st->len  -=  (int)n;
}

/**
 * Peeks the stack at item n.
 *
 * @param       st              pointer to stack
 * @param       n               peek index
 * @return                      pointer to stack data
 */
[[nodiscard]] void *peek_stack(const stack *const st, const unsigned n) {       // stack peek
    cit10a_asrt(st != nullptr && st->len > (int)n);
    return  (char*)st->data + (int)n * st->data_s;
}

/**
 * Peeks the stack at the tail.
 *
 * @param       st              pointer to stack
 * @param       n               peek index
 * @return                      pointer to stack data
 */
[[nodiscard]] void *peek_stack_tail(const stack *const st) {                    // stack peek at tail
    cit10a_asrt(st->len != 0);
    return  (char*)st->data + (int)(st->len - 1) * st->data_s;
}

/*-STACK-FREE-FUNCTIONS-----------------------------------------------------------------------------------------------*/

/**
 * Frees the stack's data; use `free_m_stack` to free a heap allocated stack.
 *
 * @param       st          pointer to stack
 */
void free_stack(stack *const st) {                                              // free stack stack
    cit10a_asrt(st != nullptr);
    safe_free(st->data);

#ifndef NDEBUG
    // struct zero
    st->len     =   0;
    st->size    =   0;
    st->data_s  =   0;
#endif  /* NDEBUG */
}

/**
 * Frees a stack on the heap. Calls `free_stack` to free the stack's data, then frees the full stack struct.
 *
 * @param       st          pointer to pointer to stack
 */
void free_m_stack(stack **const st) {                                           // free heap stack
    free_stack(*st);
    safe_free(*st);
}
