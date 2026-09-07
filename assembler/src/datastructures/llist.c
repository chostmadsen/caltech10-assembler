/**
 * src/datastructures/llist.c
 * Linked list implementation functions. These are not general linked lists; they're specialized for hashmaps.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdint.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "helpers/mem.h"
#include    "datastructures/hash.h"
#include    "datastructures/llist.h"

/*-LINKED-LIST-NODE-CREATION-FUNCTION---------------------------------------------------------------------------------*/

/**
 * Mallocs a linked list node and moves the passed pointer to the data to the node.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       key             key to link with node
 * @param       hash            hash of key.
 * @param       data            initial linked list data (should be heap allocated)
 * @param       next            new node next node (nullptr for none)
 * @return                      new linked list
 */
[[nodiscard]] ll_node *alloc_ll_node_( const src_slice *const key,
                                       const uint64_t         hash,
                                             void      *const data,
                                             ll_node   *const next      ) {     // linked list node allocation
    cit10a_asrt(key != nullptr);

    // node creation
    ll_node     *const  node    =   chckd_malloc(sizeof(ll_node), "list node ll_node*");

    // node internal setup
    node->key                   =   *key;
    node->hash                  =   hash;
    node->data                  =   data;
    node->next                  =   next;
    return  node;
}

/**
 * Frees a linked list node and its associated data.
 *
 * @param       node            node to free.
 */
void free_ll_node_(ll_node *const node) {                                       // linked list node free
    cit10a_asrt(node != nullptr);

    safe_free(node->data);

#ifndef NDEBUG
    // struct zero
    node->hash      =   0;
    node->next      =   nullptr;
    node->key.len   =   0;
#endif  /* NDEBUG */

    free(node);
}

/*-LINKED-LIST-CREATION-FUNCTIONS-------------------------------------------------------------------------------------*/

/**
 * Creates a new linked list with the header on the stack. Freeing should be done by the `free_llist` function.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       key             key to link with node
 * @param       hash            hash of key.
 * @param       data            initial linked list data (should be heap allocated)
 * @return                      new linked list
 */
[[nodiscard]] llist new_llist( const src_slice *const key,  
                               const uint64_t         hash,
                                     void      *const data  ) {                 // linked list creation on the stack
    // linked list header on stack
    llist   ret =   (llist){ .head=(data != nullptr) ? alloc_ll_node_(key, hash, data, nullptr) : nullptr,
                             .size=(data != nullptr) ? 1 : 0                                               };
    ret.tail    =   ret.head;
    return  ret;
}

/**
 * Creates a new linked list with the header on the heap. Freeing should be done by the `free_m_llist` function.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       key             key to link with node
 * @param       hash            hash of key.
 * @param       data            initial linked list data (should be heap allocated)
 * @return                      new linked list
 */
[[nodiscard]] llist *malloc_llist(const src_slice *const key,
                                  const uint64_t         hash,
                                        void      *const data  ) {              // linked list creation on the heap
    // list creation
    llist       *const  list    =   chckd_malloc(sizeof(llist), "llist llist*");

    *list   =   new_llist(key, hash, data);
    return  list;
}

/*-LINKED-LIST-DATA-MODIFIER-FUNCTIONS--------------------------------------------------------------------------------*/

/**
 * Adds an item at the specified index to the linked list.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       list            linked list to add to
 * @param       key             key to link with node
 * @param       hash            hash of key.
 * @param       data            linked list data (should be heap allocated)
 * @param       n               addition index
 */
void llist_add(       llist    *const list, const src_slice  *const key,
                const uint64_t        hash,       void       *const data,
                      size_t          n                                   ) {   // linked list node insert
    cit10a_asrt(list != nullptr && n <= list->size);
    cit10a_asrt(key->len != 0);

    // node insertion search
    ll_node    *prev    =   nullptr;
    ll_node    *curr    =   list->head;
    for (; n != 0; --n) {
        cit10a_asrt(curr != nullptr);
        prev    =   curr;
        curr    =   curr->next;
    }

    // new node creation
    ll_node     *const  node    =   alloc_ll_node_(key, hash, data, curr);
    ++list->size;

    if   (prev == nullptr)      list->head  =   node;       // head update
    else                        prev->next  =   node;       // arbitrary
    if   (curr == nullptr)      list->tail  =   node;       // tail update
}

/**
 * Adds an item at the head of the linked list.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       list            linked list to add to
 * @param       key             key to link with node
 * @param       hash            hash of key.
 * @param       data            linked list data (should be heap allocated)
 */
void llist_add_head(       llist      *const list,
                     const src_slice  *const key,
                     const uint64_t          hash,
                           void       *const data  ) {                          // linked list head insert
    cit10a_asrt(list != nullptr);
    cit10a_asrt(key->len != 0);

    // new node creation and insertion
    ll_node     *const  node    =   alloc_ll_node_(key, hash, data, list->head);

    // add at head
    if (list->head == nullptr)      list->tail  =   node;
    list->head                  =   node;
    ++list->size;
}

/**
 * Appends an item to the end of a linked list.
 * Linked list data should be heap allocated, and the list takes full ownership of the data.
 *
 * @param       list            linked list to add to
 * @param       key             key to link with node
 * @param       data            linked list data (should be heap allocated)
 */
void llist_append(       llist      *const list,
                   const src_slice  *const key,
                   const uint64_t          hash,
                         void       *const data  ) {                            // linked list node append
    cit10a_asrt(list != nullptr);
    cit10a_asrt(key->len != 0);

    // new node creation
    ll_node     *const  node    =   alloc_ll_node_(key, hash, data, nullptr);

    // append at tail
    if   (list->size++ == 0)        list->head          =   node;
    else                            list->tail->next    =   node;
    list->tail                  =   node;
    return;
}

/**
 * Removes the list node at index n and frees associated data, shifting all elements afterward one over.
 *
 * @param       list            pointer to linked list
 * @param       n               index to remove
 */
void llist_rm_n(llist *const list, size_t n) {                                  // linked list node remove (num)
    cit10a_asrt(list != nullptr && n < list->size);

    // node removal search
    ll_node    *prev    =   nullptr;
    ll_node    *curr    =   list->head;
    for (; n != 0; --n) {
        cit10a_asrt(curr != nullptr);
        prev            =   curr;
        curr            =   curr->next;
    }

    // node removal
    --list->size;
    if (prev == nullptr) {
        // head update
        list->head  =   curr->next;
    } else if (curr == nullptr) {
        // tail update
        list->tail  =   nullptr;
        prev->next  =   nullptr;
    } else {
        // arbitrary
        prev->next  =   curr->next;
    }
    free_ll_node_(curr);
}

/**
 * Removes the list node with key value key and frees associated data, shifting all elements afterward one over.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       key             remove key
 * @param       hash            hashed key
 */
void llist_rm_h(       llist     *const list,
                 const src_slice *const key,
                 const uint64_t         hash  ) {                               // linked list node remove (hash)
    cit10a_asrt(list != nullptr && key != nullptr && list->size != 0);
    cit10a_asrt(key->len != 0);

    // node removal search
    cit10a_asrt(list->head != nullptr);
    ll_node    *prev    =   nullptr;
    ll_node    *curr    =   list->head;
    while (curr->hash != hash || !srcslc_eq(key, &curr->key)) {
        prev            =   curr;
        curr            =   curr->next;
        cit10a_asrt(curr != nullptr);
    }

    // node removal
    --list->size;
    if (prev == nullptr) {
        // head update
        list->head  =   curr->next;
    } else if (curr == nullptr) {
        // tail update
        list->tail  =   nullptr;
        prev->next  =   nullptr;
    } else {
        // arbitrary
        prev->next  =   curr->next;
    }
    free_ll_node_(curr);
}

/**
 * Removes the list node with key value key and frees associated data, shifting all elements afterward one over.
 *
 * @param       list            pointer to linked list
 * @param       key             remove key
 */
void llist_rm_k(llist *const list, const src_slice *const key) {                // linked list node remove (key)
    llist_rm_h(list, key, hash_fnv1a_slc(key));
}

/**
 * Sets the data in the linked list at index n. Frees data previously at that index.
 *
 * @param       list            pointer to linked list
 * @param       data            new linked list data
 * @param       n               set index
 */
void llist_set_n(llist *const list, void *const data, size_t n) {               // linked list node set (num)
    cit10a_asrt(list != nullptr && n < list->size);

    ll_node    *itm     =   list->head;
    for (; n != 0; --n) {
        cit10a_asrt(itm->next != nullptr);
        itm             =   itm->next;
    }
    free(itm->data);
    itm->data           =   data;
}

/**
 * Sets the data in the linked list at key value key. Frees data previously at that key.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       data            new linked list data
 * @param       key             set key
 * @param       hash            hashed key
 */
void llist_set_h(       llist     *const list,       void     *const data,
                  const src_slice *const key,  const uint64_t        hash  ) {  // linked list node set (hash)
    cit10a_asrt(list != nullptr && list->size != 0 && key != nullptr);
    cit10a_asrt(key->len != 0);

    // node set search
    ll_node    *itm     =   list->head;
    while (itm->hash != hash || !srcslc_eq(key, &itm->key)) {
        cit10a_asrt(itm->next != nullptr);
        itm             =   itm->next;
    }
    free(itm->data);
    itm->data           =   data;
}

/**
 * Sets the data in the linked list at key value key. Frees data previously at that key.
 *
 * @param       list            pointer to linked list
 * @param       data            new linked list data
 * @param       key             set key
 */
void llist_set_k(       llist     *const list,
                        void      *const data,
                  const src_slice *const key   ) {                              // linked list node set (key)
    llist_set_h(list, data, key, hash_fnv1a_slc(key));
}

/*-LINKED-LIST-DATA-VIEWER-FUNCTIONS----------------------------------------------------------------------------------*/

/**
 * Peeks the linked list at index n.
 *
 * @param       list            pointer to linked list
 * @param       n               peek index
 * @return                      pointer to linked list data
 */
[[nodiscard]] void *llist_peek_n(const llist *const list, size_t n) {           // linked list peek (num)
    cit10a_asrt(list != nullptr && n < list->size);

    // get and return data
    const   ll_node    *itm     =   list->head;
    for (; n != 0; --n) {
        cit10a_asrt(itm->next != nullptr);
        itm                     =   itm->next;
    }
    return  itm->data;
}

/**
 * Peeks the linked list at key key. Returns nullptr if no node with key key is found.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       hash            hashed key
 * @return                      pointer to linked list data
 */
[[nodiscard]] void *llist_peek_h( const llist     *const list,
                                  const src_slice *const key,
                                  const uint64_t         hash  ) {              // linked list peek (hash)
    cit10a_asrt(list != nullptr && key != nullptr);
    cit10a_asrt(key->len != 0);

    // get and return data
    const   ll_node    *itm     =   list->head;
    while (itm != nullptr) {
        if (itm->hash == hash && srcslc_eq(key, &itm->key)) {
            return  itm->data;
        }
        itm                     =   itm->next;
    }
    return  nullptr;
}

/**
 * Peeks the linked list at key key. Returns nullptr if no node with key key is found.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @return                      pointer to linked list data
 */
[[nodiscard]] void *llist_peek_k( const llist     *const list,
                                  const src_slice *const key   ) {              // linked list peek (key)
    return  llist_peek_h(list, key, hash_fnv1a_slc(key));
}

/**
 * Checks the linked list at key key. Returns whether the key was found.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       hash            hashed key
 * @return                      whether the key was found
 */
bool llist_has_h( const llist    *const list, const src_slice *const key,
                  const uint64_t        hash                              ) {   // linked list check (hash)
    cit10a_asrt(list != nullptr && key != nullptr);
    cit10a_asrt(key->len != 0);

    // get and return data
    const   ll_node    *itm     =   list->head;
    while (itm != nullptr) {
        if (itm->hash == hash && srcslc_eq(key, &itm->key)) {
            return  true;
        }
        itm                     =   itm->next;
    }
    return  false;
}

/**
 * Checks the linked list at key key. Returns whether the key was found.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @return                      whether the key was found
 */
bool llist_has_k(const llist *const list, const src_slice *const key) {         // linked list check (key)
    return  llist_has_h(list, key, hash_fnv1a_slc(key));
}

/*-CAPPED-LINKED-LIST-DATA-MODIFIER-/-VIEWER-FUNCTIONS----------------------------------------------------------------*/

/**
 * Peeks the linked list at key key. Returns nullptr if no node with key key is found. Caps peek length.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       hash            hashed key
 * @param       cap             check cap
 * @return                      pointer to linked list data
 */
[[nodiscard]] void *llist_peek_h_cap( const llist     *const list,
                                      const src_slice *const key,
                                      const uint64_t         hash,
                                            size_t           cap   ) {          // capped linked list peek (hash)
    cit10a_asrt(list != nullptr && key != nullptr);
    cit10a_asrt(key->len != 0);

    // get and return data
    const   ll_node    *itm     =   list->head;
    while (cap--) {
        cit10a_asrt(itm != nullptr);
        if (itm->hash == hash && srcslc_eq(key, &itm->key)) {
            return  itm->data;
        }
        itm                     =   itm->next;
    }
    return  nullptr;
}

/**
 * Peeks the linked list at key key. Returns nullptr if no node with key key is found. Caps peek length.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       cap             check cap
 * @return                      pointer to linked list data
 */
[[nodiscard]] void *llist_peek_k_cap( const llist     *const list,
                                      const src_slice *const key,
                                      const size_t           cap   ) {          // capped linked list peek (key)
    return  llist_peek_h_cap(list, key, hash_fnv1a_slc(key), cap);
}

/**
 * Checks the linked list at key key. Returns whether the key was found. Caps peek length.
 * Uses provided hash, rather than computing it.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       hash            hashed key
 * @param       cap             check cap
 * @return                      whether the key was found
 */
bool llist_has_h_cap( const llist     *const list,
                      const src_slice *const key,
                      const uint64_t         hash,       
                            size_t           cap   ) {                          // capped linked list check (hash)
    cit10a_asrt(list != nullptr && key != nullptr);
    cit10a_asrt(key->len != 0);

    // get and return data
    const   ll_node    *itm     =   list->head;
    while (cap--) {
        cit10a_asrt(itm != nullptr);
        if (itm->hash == hash && srcslc_eq(key, &itm->key)) {
            return  true;
        }
        itm                     =   itm->next;
    }
    return  false;
}

/**
 * Checks the linked list at key key. Returns whether the key was found. Caps peek length.
 *
 * @param       list            pointer to linked list
 * @param       key             peek key
 * @param       cap             check cap
 * @return                      whether the key was found
 */
bool llist_has_k_cap( const llist     *const list, 
                      const src_slice *const key, 
                      const size_t           cap   ) {                          // capped linked list check (key)
    return  llist_has_h_cap(list, key, hash_fnv1a_slc(key), cap);
}

/*-LINKED-LIST-NODE-MOVE-FUNCTIONS------------------------------------------------------------------------------------*/

/**
 * Moves the head of src, setting the new head of src to the next element after head, to the head of dst, moving the old
 * head of dst to the item src references. (Future chost comment: this is the worst docstring ever written)
 *
 * @param       dst             destination linked list
 * @param       src             source linked list
 */
void llist_move_head(llist *const dst, llist *const src) {                      // linked list head move
    cit10a_asrt(dst != nullptr && src != nullptr);
    cit10a_asrt(src->head != nullptr);

    ll_node     *const  src_h   =   src->head;

    // move head
    src->head                   =   src_h->next;
    if (src_h->next == nullptr)     src->tail   =   nullptr;
    src_h->next                 =   dst->head;
    dst->head                   =   src_h;

    // update internals
    --src->size;
    ++dst->size;
}

/*-LINKED-LIST-FREE-FUNCTIONS-----------------------------------------------------------------------------------------*/

/**
 * Frees data stored within the linked list; use `free_m_llist` to free a heap-allocated linked list.
 *
 * @param       list            pointer to linked list
 */
void free_llist(llist *const list) {                                            // free stack linked list
    cit10a_asrt(list != nullptr);

    // no-op on empty list
    if (list->size == 0)    return;

    ll_node    *next    =   list->head;
    for (size_t i = 0; i < list->size; ++i) {
        cit10a_asrt(next != nullptr);

        ll_node    *const   rm_node =   next;
        next                        =   next->next;
        free_ll_node_(rm_node);
    }

#ifndef NDEBUG
    // struct zero
    list->size  =   0;
    list->head  =   nullptr;
    list->tail  =   nullptr;
#endif  /* NDEBUG */
}

/**
 * Frees the heap-allocated linked list header and the data within the linked list;
 * calls `free_llist`, then frees header.
 *
 * @param       list            pointer to pointer to linked list
 */
void free_m_llist(llist **const list) {                                         // free heap linked list
    cit10a_asrt(list != nullptr);
    free_llist(*list);
    safe_free(*list);
}
