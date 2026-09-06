/**
 * inc/datastructures/stack.h
 * documentation @ src/datastructures/stack.c
 */

#pragma     once

#include    <stddef.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   unsigned    DFLT_STACK_S        =   8;                              // default stack size

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // memory stack struct
            void       *data;
            int         len,    size;
  /*const*/ size_t      data_s;
} stack;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] stack new_stack_(size_t data_s, unsigned size);                   // stack creation on the stack
#define new_stack_dflt_(data_s)                 new_stack_(data_s, DFLT_STACK_S)
#define ns_pick_(data_s, _1, _fn, ...)          _fn
#define new_stack(...)                          ns_pick_(__VA_ARGS__, new_stack_, new_stack_dflt_)(__VA_ARGS__)

[[nodiscard]] stack *malloc_stack_(size_t data_s, unsigned size);               // stack creation on the heap
#define malloc_stack_dflt_(data_s)              malloc_stack_(data_s, DFLT_STACK_S)
#define ms_pick_(data_s, _1, _fn, ...)          _fn
#define malloc_stack(...)                       ms_pick_(__VA_ARGS__, malloc_stack_, malloc_stack_dflt_)(__VA_ARGS__)

void push_stack(stack *st, const void *itm);                                    // push to stack
void *pop_stack(stack *st);                                                     // pop from stack
void rm_stack(stack *st);                                                       // remove from stack
void rm_stack_n(stack *st, unsigned n);                                         // multiple removals from stack
[[nodiscard]] void *peek_stack(const stack *st, unsigned n);                    // stack peek
[[nodiscard]] void *peek_stack_tail(const stack *st);                           // stack peek at tail

void free_stack(stack *st);                                                     // free stack stack
void free_m_stack(stack **st);                                                  // free heap stack
