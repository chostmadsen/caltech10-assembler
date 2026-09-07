/**
 * src/datastructures/stackmap.c
 * Hashmaps with stack-based storage; faster (s/o cpu caches) but cannot remove items and items must be uniform.
 */

#include    <stddef.h>
#include    <stdint.h>
#include    <string.h>
#include    <limits.h>

#include    "helpers/general.h"
#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "datastructures/hash.h"
#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"

/*-INTERNAL-HASHMAP-FUNCTIONS-----------------------------------------------------------------------------------------*/

/**
 * Allocates stackmap stacks.
 *
 * @param       buckets         number of stackmap buckets
 * @return                      pointer to stackmap heads
 */
[[nodiscard]] static stack *stackmap_alloc_heads_( const size_t buckets, 
                                                   const size_t elm_s    ) {    // stackmap bucket allocator
    // bucket creation
    cit10a_asrt(!(buckets & (buckets - 1)));
    stack   *const  smap_heads          =   chckd_malloc(buckets * sizeof(stack), "stackmap stack*");
    for (size_t i = 0; i < buckets; ++i)    smap_heads[i]   =   new_stack(STACKMAP_ST_S, elm_s);
    return  smap_heads;
}

/**
 * Doubles stackmap buckets, moving data to new stacks as appropriate and freeing old stacks.
 *
 * @param       hmap            pointer to hashmap
 */
static void smap_realloc_buckets_(stackmap *const smap) {                       // stackmap bucket reallocation
    // bucket number update
    smap->buckets   <<= 1;

    // new bucket creation
    stack   *const  new_heads   =   stackmap_alloc_heads_(smap->buckets, smap->elm_s);

    // stack movement
    for (size_t i = 0; i < smap->buckets >> 1; ++i) {
        stack   *const  st      =   smap->heads + i;
        while (st->size != 0) {
            const   void    *const  itm =   pop_stack(st);
            const   size_t          loc =   ((smap_head*)itm)->hash & (smap->buckets & (smap->buckets - 1));
            push_stack(new_heads + loc, itm);
        }
    }

    // free old heads
    for (size_t i = 0; i < smap->buckets >> 1; ++i)     free_stack(smap->heads + i);
    free(smap->heads);

    // internal update
    smap->heads     =   new_heads;
}

/*-STACKMAP-CREATION-FUNCTIONS----------------------------------------------------------------------------------------*/

/**
 * Creates a new stackmap with the header on the stack. Freeing should be done by the `free_stackmap` function.
 *
 * @param       buckets         initial stackmap buckets
 * @param       elm_s           element item size
 * @return                      new stackmap
 */
[[nodiscard]] stackmap new_stackmap(const size_t buckets, const size_t elm_s) { // stackmap creation
    cit10a_asrt(buckets != 0);
    return  (stackmap){ .buckets=buckets, .elements=0, .elm_s=elm_s, .heads=stackmap_alloc_heads_(buckets, elm_s) };
}


/*-STACKMAP-DATA-MODIFIER-/-VIEWER-FUNCTIONS--------------------------------------------------------------------------*/

/**
 * Add an element to the stackmap, using memcpy.
 * IMPORTANT : The first element of a stackmap item to be pushed MUST be smap_head.
 *
 * @param       smap            stackmap
 * @param       data            data to push
 * @return                      whether the item was sucessfully added
 */
[[nodiscard]] bool stackmap_add(stackmap *const smap, const void *const data) { // stackmap kv add
    cit10a_asrt(smap != nullptr && data != nullptr);

    const   smap_head   *const  head    =   (smap_head*)data;

    // map insert index
    const   size_t  buck_n  =   head->hash & (smap->buckets - 1);
    stack   *const  st      =   smap->heads + buck_n;
    for (int i = 0; i < st->size; ++i) {
        // verify no repition
        const   smap_head   *const  s_hd                                    =   (smap_head*)peek_stack(st, i);
        if (head->hash == s_hd->hash && srcslc_eq(&head->key, &s_hd->key))      return  false;
    }

    // realloc check
    if ((float)smap->elements / (float)smap->buckets > STACKMAP_LAMBDA_MAX)     smap_realloc_buckets_(smap);

    // stack push
    push_stack(st, data);
    ++smap->elements;
    return  true;
}

/**
 * Add an element to the stackmap, using memcpy. Lowercase collision check variant; use `_lwr` lookup variants.
 * IMPORTANT : The first element of a stackmap item to be pushed MUST be smap_head.
 *
 * @param       smap            stackmap
 * @param       data            data to push
 * @return                      stackmap collision type
 */
[[nodiscard]] smap_clsn_t stackmap_add_lwr(       stackmap *const smap,
                                            const void     *const data  ) {     // stackmap kv add (lower)
    cit10a_asrt(smap != nullptr && data != nullptr);

    smap_clsn_t                 ret     =   smap_no_clsn_t;
    const   smap_head   *const  head    =   (smap_head*)data;

    // map insert index
    const   size_t  buck_n  =   head->hash & (smap->buckets - 1);
    stack   *const  st      =   smap->heads + buck_n;
    for (int i = 0; i < st->size; ++i) {
        const   smap_head   *const  s_hd            =   (smap_head*)peek_stack(st, i);
        if (head->hash != s_hd->hash)                   continue;
        if (srcslc_eq(&head->key, &s_hd->key))          return  smap_full_clsn_t;
        if (srcslc_eq_lwr(&head->key, &s_hd->key))      ret =   smap_lwr_clsn_t;
    }

    // realloc check
    if ((float)smap->elements / (float)smap->buckets > STACKMAP_LAMBDA_MAX)     smap_realloc_buckets_(smap);

    // stack push
    push_stack(st, data);
    ++smap->elements;
    return  ret;
}

/**
 * Gets the value at the key in the stackmap. Returns nullptr if no key could be found. Check the pointer for nullptr
 * for the stackmap equivalent of `hashmap_has`.
 * Uses provided hash, rather than computing it.
 *
 * @param       smap            stackmap
 * @param       key             search key
 * @param       hash            hashed key
 * @return                      stackmap data void*
 */
[[nodiscard]] void *stackmap_get_h( const stackmap  *const smap,
                                    const src_slice *const key,
                                    const uint64_t         hash  ) {             // stackmap item get (hash)
    cit10a_asrt(smap != nullptr && key != nullptr);

    // map insert index
    const   size_t  buck_n  =   hash & (smap->buckets - 1);
    stack   *const  st      =   smap->heads + buck_n;

    // get stackmap data
    for (int i = 0; i < st->size; ++i) {
        const   smap_head   *const  s_hd                    =   (smap_head*)peek_stack(st, i);
        if (s_hd->hash == hash && srcslc_eq(&s_hd->key, key))   return  peek_stack(st, i);
    }
    return  nullptr;
}

/**
 * Gets the value at the key in the stackmap. Returns nullptr if no key could be found. Check the pointer for nullptr
 * for the stackmap equivalent of `hashmap_has`.
 *
 * @param       smap            stackmap
 * @param       key             search key
 * @return                      stackmap data void*
 */
[[nodiscard]] void *stackmap_get_k( const stackmap  *const smap,
                                    const src_slice *const key   ) {            // stackmap item get (key)
    return  stackmap_get_h(smap, key, hash_fnv1a_slc(key));
}

/**
 * Gets the value at the key in the stackmap. Returns nullptr if no key could be found. Check the pointer for nullptr
 * for the stackmap equivalent of `hashmap_has`. Lowercase variant.
 *
 * @param       smap            stackmap
 * @param       key             search key
 * @return                      stackmap data void*
 */
[[nodiscard]] void *stackmap_get_k_lwr( const stackmap  *const smap,
                                        const src_slice *const key   ) {        // stackmap item get (key, lower)
    return  stackmap_get_h(smap, key, hash_fnv1a_slc_lwr(key));
}

/*-STACKMAP-FREE-FUNCTIONS--------------------------------------------------------------------------------------------*/

/**
 * Frees stackmap and associated data.
 *
 * @param       smap        stackmap
 */
void free_stackmap(stackmap *const smap) {                                      // free stackmap
    cit10a_asrt(smap != nullptr);

    // free stacks
    for (size_t i = 0; i < smap->buckets; ++i)  free_stack(smap->heads + i);
    safe_free(smap->heads);

#ifndef NDEBUG
    // struct zero
    smap->buckets   =   0;
    smap->elements  =   0;
    smap->elm_s     =   0;
#endif  /* NDEBUG */
}
