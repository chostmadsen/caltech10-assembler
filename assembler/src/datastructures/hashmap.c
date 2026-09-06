/**
 * src/datastructures/hashmap.c
 * Hashmap implementation functions.
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
#include    "datastructures/hashmap.h"

/*-INTERNAL-HASHMAP-FUNCTIONS-----------------------------------------------------------------------------------------*/

/**
 * Allocates hashmap linked lists.
 *
 * @param       buckets         number of hashmap buckets
 * @return                      pointer to hashmap heads
 */
[[nodiscard]] static llist *hashmap_alloc_buckets_(const size_t buckets) {      // hashmap bucket allocator
    // bucket creation
    acama_asrt(!(buckets & (buckets - 1)));
    llist   *const  new_heads   =   chckd_malloc(buckets * sizeof(llist), "hashmap head llist*");

    for (size_t i = 0; i < buckets; ++i)    *(new_heads + i)    =   new_llist(nullptr, 0, nullptr);
    return  new_heads;
}

/**
 * Doubles hashmap buckets, moving node pointers to the appropriate bucket and freeing the old linked list header.
 *
 * @param       hmap            pointer to hashmap
 */
static void realloc_buckets_(hashmap *const hmap) {                             // hashmap bucket reallocation
    // bucket number update
    hmap->buckets   <<= 1;

    // new bucket creation
    llist   *const  new_heads   =   hashmap_alloc_buckets_(hmap->buckets);

    // node movement
    for (size_t i = 0; i < hmap->buckets >> 1; ++i) {
        llist   *const  list    =   hmap->heads + i;
        while (list->size != 0) {
            const   size_t  loc =   list->head->hash & (hmap->buckets - 1);
            llist_move_head(new_heads + loc, list);
        }
    }

    // internal update
    free(hmap->heads);
    hmap->heads     =   new_heads;
}

/*-HASHMAP-CREATION-FUNCTIONS-----------------------------------------------------------------------------------------*/

/**
 * Creates a new hashmap with the header on the stack. Freeing should be done by the `free_hashmap` function.
 *
 * @param       buckets         initial buckets (must be 2^n)
 * @return                      new hashmap
 */
[[nodiscard]] hashmap new_hashmap(const size_t buckets) {                       // hashmap creation on stack
    // hashmap internal setup
    return  (hashmap){ .heads=hashmap_alloc_buckets_(buckets), .buckets=DFLT_BUCKETS, .elements=0 };
}

/**
 * Creates a new hashmap with the header on the heap. Freeing should be done by the `free_m_hashmap` function.
 *
 * @param       buckets         initial buckets (must be 2^n)
 * @return                      pointer to new hashmap
 */
[[nodiscard]] hashmap *malloc_hashmap(const size_t buckets) {                   // hashmap creation on heap
    // hashmap malloc
    hashmap *const  hmap        =   chckd_malloc(sizeof(hashmap), "hashmap hashmap");

    *hmap   =   new_hashmap(buckets);
    return  hmap;
}

/*-HASHMAP-DATA-MODIFIER-/-VIEWER-FUNCTIONS---------------------------------------------------------------------------*/

/**
 * Gets the value at the key in the hashmap. Returns nullptr if no key could be found.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       hash            hashed key
 * @return                      pointer to hashmap value
 */
[[nodiscard]] void *hashmap_get_h( const hashmap   *const hmap,
                                   const src_slice *const key,
                                   const uint64_t         hash  ) {             // hashmap value get (hash)
    acama_asrt(hmap != nullptr && key != nullptr);
    acama_asrt(hmap->heads != nullptr);

    // get key val
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);
    return  llist_peek_h(hmap->heads + buck_n, key, hash);
}

/**
 * Gets the value at the key in the hashmap. Returns nullptr if no key could be found.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @return                      pointer to hashmap value
 */
[[nodiscard]] void *hashmap_get_k( const hashmap   *const hmap,
                                   const src_slice *const key   ) {             // hashmap value get (key)
    return  hashmap_get_h(hmap, key, hash_fnv1a_slc(key));
}

/**
 * Checks if the key is in the hashmap.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       hash            hashed key
 * @return                      whether the key is in the hashmap
 */
bool hashmap_has_h( const hashmap   *const hmap,
                    const src_slice *const key,
                    const uint64_t         hash  ) {                            // hashmap check (hash)
    acama_asrt(hmap != nullptr && key != nullptr);
    acama_asrt(hmap->heads != nullptr);

    // check hashmap
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);
    return  llist_has_h(hmap->heads + buck_n, key, hash);
}

/**
 * Checks if the key is in the hashmap.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @return                      whether the key is in the hashmap
 */
bool hashmap_has_k(const hashmap *const hmap, const src_slice *const key) {     // hashmap check (key)
    return  hashmap_has_h(hmap, key, hash_fnv1a_slc(key));
}

/**
 * Removes the hashmap item with the given key and frees associated data.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             remove key
 * @param       hash            hashed key
 */
void hashmap_rm_h(       hashmap   *const hmap,
                   const src_slice *const key,
                   const uint64_t         hash  ) {                             // hashmap item remove (hash)
    acama_asrt(hmap != nullptr && key != nullptr);

    // remove key val
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);
    llist_rm_h(hmap->heads + buck_n, key, hash);
    --hmap->elements;
}

/**
 * Removes the hashmap item with the given key and frees associated data.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             remove key
 */
void hashmap_rm_k(hashmap *const hmap, const src_slice *const key) {            // hashmap item remove (key)
    hashmap_rm_h(hmap, key, hash_fnv1a_slc(key));
}

/**
 * Adds a key-value pair to the hashmap. Invalid key pushes should be externally handled.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             new key
 * @param       val             associated key value
 * @param       hash            hashed key
 */
void hashmap_add_h( hashmap *const hmap, const src_slice *const key, 
                    void    *const val,  const uint64_t         hash ) {        // hashmap kv add (hash)
    acama_asrt(hmap != nullptr && key != nullptr);

    // internal update
    if ((float)++hmap->elements / (float)hmap->buckets > LAMBDA_MAX)        realloc_buckets_(hmap);
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);

    // externally address this for user input
    acama_asrt(!hashmap_has_h(hmap, key, hash));

    // add val
    llist_append(hmap->heads + buck_n, key, hash, val);
}

/**
 * Adds a key-value pair to the hashmap. Invalid key pushes should be externally handled.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             new key
 * @param       val             associated key value
 */
void hashmap_add_k(       hashmap   *const hmap,
                    const src_slice *const key, 
                          void      *const val   ) {                            // hashmap kv add (key)
    hashmap_add_h(hmap, key, val, hash_fnv1a_slc(key));
}

/*-HASHMAP-CAPPED-FUNCTIONS-------------------------------------------------------------------------------------------*/

/**
 * Gets the caps of a hashmap for a capped lookup.
 *
 * @param       hmap            pointer to hashmap
 * @return                      hashmap caps
 */
[[nodiscard]] hmap_caps get_caps(const hashmap *const hmap) {                   // hashmap cap get
    acama_asrt(hmap != nullptr);

    // find caps
    hmap_caps   caps    =   { .buckets=hmap->buckets };
    caps.caps           =   chckd_malloc(hmap->buckets * sizeof(size_t), "caps size_t*");
    for (size_t i = 0; i < hmap->buckets; ++i)  caps.caps[i]    =   hmap->heads[i].size;
    return  caps;
}

/**
 * Gets the value at the key in the hashmap. Returns nullptr if no key could be found. Caps off at the provided cap.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       hash            hashed key
 * @param       caps            hashmap caps
 * @return                      pointer to hashmap value
 */
[[nodiscard]] void *hashmap_get_h_cap( const hashmap   *const hmap,
                                       const src_slice *const key,
                                       const uint64_t         hash,
                                       const hmap_caps *const caps  ) {         // capped hashmap value get (hash)
    acama_asrt(hmap != nullptr && key != nullptr);
    acama_asrt(hmap->heads != nullptr);

    // get key val
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);
    return  llist_peek_h_cap(hmap->heads + buck_n, key, hash, caps->caps[buck_n]);
}
/**
 * Gets the value at the key in the hashmap. Returns nullptr if no key could be found. Caps off at the provided cap.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       caps            hashmap caps
 * @return                      pointer to hashmap value
 */
[[nodiscard]] void *hashmap_get_k_cap( const hashmap   *const hmap,
                                       const src_slice *const key,
                                       const hmap_caps *const caps  ) {         // capped hashmap value get (key)
    return  hashmap_get_h_cap(hmap, key, hash_fnv1a_slc(key), caps);
}

/**
 * Checks if the key is in the hashmap. Caps off at the provided cap.
 * Uses provided hash, rather than computing it.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       hash            hashed key
 * @param       caps            hashmap caps
 * @return                      whether the key is in the hashmap
 */
bool hashmap_has_h_cap( const hashmap   *const hmap,
                        const src_slice *const key,
                        const uint64_t         hash,
                        const hmap_caps *const caps  ) {                        // capped hashmap check (hash)
    acama_asrt(hmap != nullptr && key != nullptr);
    acama_asrt(hmap->heads != nullptr);

    // check hashmap
    const   size_t      buck_n  =   hash & (hmap->buckets - 1);
    return  llist_has_h_cap(hmap->heads + buck_n, key, hash, caps->caps[buck_n]);
}

/**
 * Checks if the key is in the hashmap. Caps off at the provided cap.
 *
 * @param       hmap            pointer to hashmap
 * @param       key             search key
 * @param       caps            hashmap caps
 * @return                      whether the key is in the hashmap
 */
bool hashmap_has_k_cap( const hashmap   *const hmap,
                        const src_slice *const key,
                        const hmap_caps *const caps  ) {                        // capped hashmap check (key)
    return  hashmap_has_h_cap(hmap, key, hash_fnv1a_slc(key), caps);
}

/*-HASHMAP-FREE-FUNCTIONS---------------------------------------------------------------------------------------------*/

/**
 * Frees linked lists and their data that store all hashmap information;
 * use `free_m_hashmap` to free a heap-allocated hashmap.
 *
 * @param       hmap        pointer to hashmap
 */
void free_hashmap(hashmap *const hmap) {                                        // free stack hashmap
    acama_asrt(hmap != nullptr);

    for (size_t i = 0; i < hmap->buckets; ++i)      free_llist(hmap->heads + i);
    safe_free(hmap->heads);

#ifndef NDEBUG
    // struct zero
    hmap->buckets   =   0;
    hmap->elements  =   0;
#endif  /* NDEBUG */
}

/**
 * Frees the heap-allocated hashmap header and the linked lists and their data that store all hashmap information;
 * calls `free_hashmap`, then frees header.
 *
 * @param       hmap        pointer to pointer to hashmap
 */
void free_m_hashmap(hashmap **const hmap) {                                     // free heap hashmap
    acama_asrt(hmap != nullptr);
    free_hashmap(*hmap);
    safe_free(*hmap);
}
