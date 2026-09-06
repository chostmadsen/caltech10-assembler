/**
 * inc/datastructures/llist.h
 * documentation @ src/datastructures/llist.c
 */

#pragma     once

#include    <stddef.h>
#include    <stdint.h>

#include    "helpers/general.h"

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct ll_node {                                                        // linked list node
            uint64_t    hash;
    struct  ll_node    *next;
            src_slice   key;   // NOTE : modify for non source char*
            void       *data;
} ll_node;

typedef struct {                                                                // linked list
            ll_node    *head;
            ll_node    *tail;
            size_t      size;
} llist;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] llist new_llist(const src_slice *key, uint64_t hash, void *data); // linked list creation on the stack
[[nodiscard]] llist *malloc_llist( const src_slice *key,
                                         uint64_t   hash, 
                                         void      *data  );                    // linked list creation on the heap

void llist_add( llist    *list, const src_slice *key,
                uint64_t  hash,       void      *data, size_t n);               // linked list node insert
void llist_add_head( llist    *list, const src_slice *key,
                     uint64_t  hash,       void      *data );                   // linked list head insert
void llist_append(llist *list, const src_slice *key, uint64_t hash, void *data);// linked list node append

void llist_rm_n(llist *list, size_t n);                                         // linked list node remove (num)
void llist_rm_h(llist *list, const src_slice *key, uint64_t hash);              // linked list node remove (hash)
void llist_rm_k(llist *list, const src_slice *key);                             // linked list node remove (key)

void llist_set_n(llist *list, void *data, size_t n);                            // linked list node set (num)
void llist_set_h(llist *list, void *data, const src_slice *key, uint64_t hash); // linked list node set (hash)
void llist_set_k(llist *list, void *data, const src_slice *key);                // linked list node set (key)

[[nodiscard]] void *llist_peek_n(const llist *list, size_t n);                  // linked list peek (num)
[[nodiscard]] void *llist_peek_h( const llist     *list,
                                  const src_slice *key, 
                                        uint64_t   hash  );                     // linked list peek (hash)
[[nodiscard]] void *llist_peek_k(const llist *list, const src_slice *key);      // linked list peek (key)

bool llist_has_h(const llist *list, const src_slice *key, uint64_t hash);       // linked list check (hash)
bool llist_has_k(const llist *list, const src_slice *key);                      // linked list check (key)

[[nodiscard]] void *llist_peek_h_cap( const llist     *list,
                                      const src_slice *key,
                                            uint64_t   hash,
                                            size_t     cap   );                 // capped linked list peek (hash)
[[nodiscard]] void *llist_peek_k_cap( const llist     *list,
                                      const src_slice *key,
                                            size_t     cap   );                 // capped linked list peek (key)

bool llist_has_h_cap( const llist    *list, const src_slice *key, 
                            uint64_t  hash,       size_t     cap  );            // linked list check (hash)
bool llist_has_k_cap(const llist *list, const src_slice *key, size_t cap);      // linked list check (key)

void llist_move_head(llist *dst, llist *src);                                   // linked list head move

void free_llist(llist *list);                                                   // free stack linked list
void free_m_llist(llist **list);                                                // free heap linked list
