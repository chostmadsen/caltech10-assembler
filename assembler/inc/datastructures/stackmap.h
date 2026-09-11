/**
 * inc/datastructures/stackmap.h
 * documentation @ src/datastructures/stackmap.c
 */

#pragma     once

#include    <stddef.h>
#include    <stdint.h>

#include    "helpers/general.h"
#include    "datastructures/stack.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   unsigned    STACKMAP_ST_S       =   4;                              // initial stackmap stack size
constexpr   float       STACKMAP_LAMBDA_MAX =   0.5f;                           // maximum load factor

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // stackmap header
            uint64_t    hash;
            src_slice   key;
} smap_head;

typedef struct {                                                                // stackmap struct
            size_t      buckets,    elements,   elm_s;
            stack      *heads;
            bool        aligned;
} stackmap;

typedef enum {                                                                  // stackmap collision type
    smap_no_clsn_t=0,   smap_lwr_clsn_t,    smap_full_clsn_t
} smap_clsn_t;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] stackmap new_stackmap(size_t buckets, size_t elm_s);              // stackmap creation
[[nodiscard]] stackmap new_stackmap_aln(size_t buckets, size_t elm_s);          // aligned stackmap creation

[[nodiscard]] bool stackmap_add(stackmap *smap, const void *data);              // stackmap kv add
[[nodiscard]] smap_clsn_t stackmap_add_lwr(stackmap *smap, const void *data);   // stackmap kv add (lower)
[[nodiscard]] void *stackmap_get_h( const stackmap   *smap,
                                    const src_slice *key,
                                          uint64_t   hash  );                   // stackmap item get (hash)
[[nodiscard]] void *stackmap_get_h_lwr( const stackmap  *smap,
                                        const src_slice *key,
                                              uint64_t   hash  );               // stackmap item get lower (hash)
[[nodiscard]] void *stackmap_get_k(const stackmap *smap, const src_slice *key); // stackmap item get (key)
[[nodiscard]] void *stackmap_get_k_lwr( const stackmap *smap, 
                                        const src_slice *key  );                // stackmap item get (key, lower)
[[nodiscard]] void *stackmap_get_k_lwr_lwr( const stackmap  *smap,
                                            const src_slice *key   );           // stackmap item get lower (key, lower)

void free_stackmap(stackmap *smap);                                             // free stackmap

void print_smap_head(const void *head);                                         // stackmap head printer
void print_stackmap(const stackmap *smap, void (*prnt_fn)(const void*));        // stackmap printer
