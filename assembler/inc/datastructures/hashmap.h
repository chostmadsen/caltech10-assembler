/**
 * inc/datastructures/hashmap.h
 * documentation @ src/datastructures/hashmap.c
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"
#include    "datastructures/llist.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   size_t      DFLT_BUCKETS        =   16;                             // default hashmap buckets
constexpr   float       LAMBDA_MAX          =   0.75f;                          // maximum load factor

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // hashmap struct
            llist      *heads;
            size_t      buckets,    elements;
} hashmap;

typedef struct {                                                                // hashmap caps struct
            size_t      buckets;
            size_t     *caps;
} hmap_caps;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] hashmap new_hashmap(size_t buckets);                              // hashmap creation on stack
[[nodiscard]] hashmap *malloc_hashmap(size_t buckets);                          // hashmap creation on heap

[[nodiscard]] void *hashmap_get_h( const hashmap   *hmap,
                                   const src_slice *key,
                                         uint64_t   hash  );                    // hashmap value get (hash)
[[nodiscard]] void *hashmap_get_k(const hashmap *hmap, const src_slice *key);   // hashmap value get (key)

bool hashmap_has_h(const hashmap *hmap, const src_slice *key, uint64_t hash);   // hashmap check (hash)
bool hashmap_has_k(const hashmap *hmap, const src_slice *key);                  // hashmap check (key)

void hashmap_rm_h(hashmap *hmap, const src_slice *key, uint64_t hash);          // hashmap item remove (hash)
void hashmap_rm_k(hashmap *hmap, const src_slice *key);                         // hashmap item remove (key)

void hashmap_add_h( hashmap *hmap, const src_slice *key,
                    void    *val,        uint64_t   hash );                     // hashmap kv add (hash)
void hashmap_add_k(hashmap *hmap, const src_slice *key, void *val);             // hashmap kv add (key)

[[nodiscard]] hmap_caps get_caps(const hashmap *hmap);                          // hashmap cap get

[[nodiscard]] void *hashmap_get_h_cap( const hashmap   *hmap,
                                       const src_slice *key,
                                             uint64_t   hash,
                                       const hmap_caps *caps  );                // capped hashmap value get (hash)
[[nodiscard]] void *hashmap_get_k_cap( const hashmap   *hmap,
                                       const src_slice *key,
                                       const hmap_caps *caps  );                // capped hashmap value get (key)

bool hashmap_has_h_cap( const hashmap   *hmap,
                        const src_slice *key,
                              uint64_t   hash,
                        const hmap_caps *caps  );                               // capped hashmap check (hash)
bool hashmap_has_k_cap( const hashmap   *hmap,
                        const src_slice *key,
                        const hmap_caps *caps  );                               // capped hashmap check (key)

void free_hashmap(hashmap *hmap);                                               // free stack hashmap
void free_m_hashmap(hashmap **hmap);                                            // free heap hashmap
