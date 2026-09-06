/**
 * inc/datastructures/vstack.h
 * documentation @ src/datastructures/vstack.c
 */

#pragma     once

#include    <stddef.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   unsigned    DFLT_VSTACK_S    =   8;                                 // default vstack size

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // vstack struct
            size_t     *data;
            int         len,    size;
} vstack;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] vstack new_vstack(unsigned size);                                 // vstack creation on the stack
[[nodiscard]] vstack *malloc_vstack(unsigned size);                             // vstack creation on the heap

void push_vstack(vstack *vst, size_t val);                                      // push to vstack
size_t pop_vstack(vstack *vst);                                                 // pop from vstack
void rm_vstack(vstack *vst);                                                    // remove from vstack
void rm_vstack_n(vstack *vst, unsigned n);                                      // multiple removals from vstack
[[nodiscard]] size_t peek_vstack(const vstack *vst, unsigned n);                // vstack peek
[[nodiscard]] size_t peek_vstack_tail(const vstack *vst);                       // vstack peek at tail

void free_vstack(vstack *vst);                                                  // free vstack
void free_m_vstack(vstack **vst);                                               // free heap vstack
