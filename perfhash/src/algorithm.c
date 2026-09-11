/**
 * perfhash/src/algorithm.c
 * Perfect hashing algorithm implementation.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <stdint.h>
#include    <inttypes.h>
#include    <limits.h>
#include    <math.h>
#ifndef NTHREAD
#include    <stdatomic.h>
#include    <pthread.h>
#include    <unistd.h>
#endif  /* NTHREAD */

#include    "general_phs.h"
#include    "hash_populate.h"
#include    "algorithm.h"

/*-RANDOM-FUNCTIONS---------------------------------------------------------------------------------------------------*/

/**
 * 64-bit random number. Uses the splitmix64 pseudo-random number generation algorithm.
 *
 * @return                      64-bit rand
 */
[[nodiscard]] static uint64_t rand64_(uint64_t *const restrict prng_state) {    // 64-bit rand
    *prng_state     +=  0x9e3779b97f4a7c15ull;
    uint64_t    z   =   (*prng_state ^ (*prng_state >> 30)) * 0xbf58476d1ce4e5b9ull;
    z               =   (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return  z ^ (z >> 31);
}

/*-STRING-MAP-CREATION-FUNCTIONS--------------------------------------------------------------------------------------*/

/**
 * String map creation; stores flat buffer of strings to hash w/o unneeded information for the algorithm.
 *
 * @param       group           hash group
 * @return                      string map
 */
[[nodiscard]] static str_map str_map_make_(const hash_grp *const group) {       // string map creation
    // start map
    str_map     map     =   { .size=group->len };

    // make strings
    size_t      str_idx =   0;
    size_t      cap     =   1;
    char       *strs    =   s_malloc(cap * sizeof(char));
    for (size_t i = 0; i < group->len; ++i) {
        const   char   *str =   group->arr[i]->str;

        while (*str != '\0') {
            if (str_idx >= cap) {
                cap     *=  2;
                strs    =   s_realloc(strs, cap * sizeof(char));
            }
            *(strs + str_idx++)     =   *(str++);
        }
        if (str_idx >= cap) {
            cap     *=  2;
            strs    =   s_realloc(strs, cap * sizeof(char));
        }
        *(strs + str_idx++)     =   '\0';
    }
    map.strs    =   strs;
    return  map;
}

/**
 * Free string map.
 *
 * @param       map             string map
 */
static void free_str_map_(str_map *const map) {                                 // string map free
    free(map->strs);
}

/*-BITSET-FUNCTIONS---------------------------------------------------------------------------------------------------*/

/**
 * Bitset bit set.
 *
 * @param       set             bitset
 * @param       n               bit to set
 * @return                      whether the bit was not set
 */
[[nodiscard]] static bool set_bitset_(       bitset   *const restrict set,
                                       const unsigned                 n    ) {  // set bitset
    // find mask item
    const   size_t      mask_idx    =   n >> 6;
    const   uint64_t    shft        =   (uint64_t)1 << (n & 63);
    const   bool        is_set      =   set->mask[mask_idx] & shft;
    set->mask[mask_idx]             |=  shft;
    return  !is_set;
}

/**
 * Bitset clear. `memset` is slower than the manual loop.
 *
 * @param       set             bitset
 */
static void clear_bitset_(bitset *const restrict set) {                         // clear bitset
    // memset(set->mask, 0, sizeof(uint64_t) * set->size);
    for (size_t i = 0; i < set->size; ++i)      set->mask[i]  =   0;
}

/**
 * New bitset creation (aligned).
 *
 * @param       n               number of bits in the bitset
 * @return                      bitset
 */
[[nodiscard]] static bitset new_bitset_(const size_t n) {                       // create bitset
    size_t  masks   =   0;
    for (int i = (int)n; i > 0; i -= 64, ++masks);
    bitset  bset    =   (bitset){ .mask=s_aln_alloc(masks * sizeof(uint64_t)), .size=masks };
    clear_bitset_(&bset);
    return  bset;
}

/*-PERFECT-HASHING-ALGORITHM------------------------------------------------------------------------------------------*/

/**
 * Modified FNV-1a hash.
 * The bottom bit fold helps with finding the hashing offset faster.
 *
 * @param       str             string to hash
 * @param       offset          fnv-1a offset
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fn_impl(const char *str, const uint64_t offset) {   // hash function
    uint64_t    hash    =   offset;
    while (*str != '\0') {
        hash    ^=  (unsigned char)*(str++);
        hash    *=  FNV_PRIME_64;
    }
    return  hash ^ (hash >> 32);
}

/**
 * Hashing on a string slice, instead of hash table objects.
 * The flat buffer slightly reduces the amount of L1 cache missing.
 *
 * @param       str             string pointer to hash
 * @param       offset          fnv-1a offset
 * @return                      hash
 */
[[nodiscard]] static uint64_t hash_fn_fast_( const char     *restrict
                                                            *const
                                                             restrict str,
                                                   uint64_t           offset ) {// fast hash function
    while (**str != '\0') {
        offset  ^=  (unsigned char)*((*str)++);
        offset  *=  FNV_PRIME_64;
    }
    ++(*str);
    return  offset ^ (offset >> 32);
}

/**
 * Perfect hash probability.
 *
 * @param       n               items
 * @param       buckets         bucket number
 * @param       attempts        attempts
 * @return                      probability
 */
[[nodiscard]] static double perf_hash_p_( const size_t n,
                                          const size_t buckets,
                                          const size_t attempts ) {             // perfect hash probability
    double  single_p    =   1.0;
    for (size_t k = 0; k < n; ++k) {
        single_p        *=  1.0 - ((double)k / (double)buckets);
    }
    
    return  1.0 - pow(1.0 - single_p, (double)attempts);
}

/*-PERFECT-HASHING-ALGORITHM-OUTPUT-----------------------------------------------------------------------------------*/

/**
 * Attempt header output.
 * 
 * @param       items           number of items to bucket
 * @param       buckets         buckets
 * @param       max_attempt     maximum attempt
 */
static void attempt_header_prnt_( const size_t   items,
                                  const size_t   buckets,
                                  const size_t   max_attempt ) {               // attempt header
    printf( "%'zu -> %'zu [ %.8g %% ]\n",
            items, buckets, perf_hash_p_(items, buckets, max_attempt) * 100);
}

/**
 * Trial header output.
 *
 * @param       offset          fnv-1a offset
 * @param       attempt         attempt
 * @param       max_attempt     maximum attempt
 * @param       flush           flush stdout
 */
static void trial_header_prnt_( const uint64_t offset,
                                const size_t   attempt,
                                const size_t   max_attempt,
                                const bool     flush        ) {                 // trial header
    printf("\r\x1b[2K0x%016" PRIx64 " - %'zu / %'zu", offset, attempt, max_attempt);
    if (flush)      fflush(stdout);
}

/*-PERFECT-HASHING-ALGORITHM-TRIAL-(SINGLE-THREADED)------------------------------------------------------------------*/

static      bool                nt_term     =   false;                          // non-threaded termination flag

/**
 * Single perfect hashing trial to a maximum attempt.
 *
 * @param       map             string map
 * @param       seed            initial random seed
 * @param       max_attempt     maximum attempts
 * @param       buckets         buckets
 * @return                      successful offset or zero on fail
 */
[[maybe_unused]] [[nodiscard]]
static uint64_t trial_( const str_map  map,
                        const uint64_t seed,
                        const size_t   max_attempt,
                        const size_t   buckets      ) {                         // single trial instance
    // random seeding
    uint64_t                    prng_st =   seed;
    uint64_t                    offset  =   rand64_(&prng_st);

    // pointer setup
    size_t                      attempt =   0;
    const   char    *restrict   str_ptr =   map.strs;

    // biset setup
    bitset                      set     =   new_bitset_(buckets);

    // hash search
    bool                        found   =   true;
    trial_header_prnt_(offset, attempt, max_attempt, true);
    for (size_t i = 0; i < map.size; ++i) {
        // hash and bucket
        const   uint64_t    hash    =   hash_fn_fast_(&str_ptr, offset);
        if (set_bitset_(&set, (size_t)hash & (buckets - 1)))    continue;

        // reset
        offset      =   rand64_(&prng_st);
        i           =   (size_t)-1;
        str_ptr     =   map.strs;
        clear_bitset_(&set);

        // attempt check
        if (!(attempt & 0xffff))                                trial_header_prnt_(offset, attempt, max_attempt, true);
        if (++attempt >= max_attempt) {
            found   =   false;
            goto    trial_end;
        }
    }

trial_end:
    // end
    trial_header_prnt_(offset, attempt, max_attempt, false);
    printf(" [ %s on %" PRIu64 " ]\n", (found) ? "success" : "fail", seed);
    if (found)          nt_term =   true;
    free(set.mask);
    return  (found) ? offset : 0;
}

/*-PERFECT-HASHING-ALGORITHM-THREAD-HANDLER---------------------------------------------------------------------------*/

#ifndef NTHREAD

static      atomic_bool         terminate   =   false;                          // threaded termination flag
static      uint64_t            offset_t    =   0x0;                            // final offset
static      uint64_t            seed_t      =   0x0;                            // final seed

typedef struct {                                                                // multithreaded trial data
    const   str_map            *map;
            uint64_t            seed;
            size_t              max_attempt;
            size_t              buckets;
            unsigned            stride;
} trial_data;

/**
 * Multithreaded hashing trial to a maximum attempt before rotation.
 *
 * @param       arg             trial_data (mirrors single-threaded arg)
 * @return                      nullptr
 */
[[maybe_unused]] static void *trial_multi_(void *const arg) {                   // multithreaded trial instance
    const   trial_data  *const  data    =   arg;

    // random seeding
    uint64_t                    seed    =   data->seed;
    uint64_t                    prng_st =   seed;
    uint64_t                    offset  =   rand64_(&prng_st);

    // pointer setup
    size_t                      attempt =   0;
    const   char    *restrict   str_ptr =   data->map->strs;

    // biset setup
    bitset                      set     =   new_bitset_(data->buckets);

    // hash search
    const   size_t              buckets =   data->buckets;
    const   size_t              max     =   data->max_attempt;
    trial_header_prnt_(offset, attempt, max, true);
    const   size_t              nitms   =   data->map->size;
    const   char               *out_str =   "terminated";
    for (size_t i = 0; i < nitms; ++i) {
        // hash and bucket
        const   uint64_t    hash    =   hash_fn_fast_(&str_ptr, offset);
        if (set_bitset_(&set, (size_t)hash & (buckets - 1)))            continue;

        // reset
        offset      =   rand64_(&prng_st);
        i           =   (size_t)-1;
        str_ptr     =   data->map->strs;
        clear_bitset_(&set);

        // attempt check
        if (!(attempt & 0xffff)) {
            trial_header_prnt_(offset, attempt, max, true);
            if (atomic_load_explicit(&terminate, memory_order_relaxed)) goto    thread_terminate;
        }

        // reset check
        if (++attempt >= max) {
            // failure output
            flockfile(stdout);
            trial_header_prnt_(offset, attempt, max, false);
            printf(" [ fail on %" PRIu64 " ]\n", seed);
            funlockfile(stdout);

            // full reset
            attempt =   0;
            seed    +=  data->stride;
            prng_st =   seed;
            offset  =   rand64_(&prng_st);
        }
    }

    // store check
    if (!atomic_exchange_explicit(&terminate, true, memory_order_relaxed)) {
        // NOTE : modify offset_t if not terminating here
        offset_t    =   offset;
        seed_t      =   seed;
        out_str     =   "success";
    }

thread_terminate:
    // end trial
    free(set.mask);
    flockfile(stdout);
    trial_header_prnt_(offset, attempt, max, false);
    printf(" [ %s on %" PRIu64 " ]\n", out_str, seed);
    funlockfile(stdout);
    return  nullptr;
}

/**
 * Perfect hashing algorithm. Called by `algorithm` if multithreading is not disabled.
 *
 * @param       group           hash group
 * @param       seed            initial seed offset
 * @param       max_attempt     maximum attempt before seed rotation
 * @param       offset          initial offset
 */
[[maybe_unused]] [[nodiscard]]
static hash_fn alg_orch_( const hash_grp *const group,
                                uint64_t        seed,
                          const size_t          max_attempt,
                                size_t          offset       ) {                // perfect hash algorithm orchestrator
    // general init
    const   size_t      buckets =   group->size << offset;
    const   long        online  =   sysconf(_SC_NPROCESSORS_ONLN);
    const   unsigned    thrd_n  =   (online > 0) ? (unsigned)online : 1u;

    // string map init
    str_map             map     =   str_map_make_(group);

    // clear items
    atomic_store_explicit(&terminate, false, memory_order_relaxed);
    offset_t                    =   0x0;

    // thread data init
    trial_data  *const  data    =   s_malloc(thrd_n * sizeof(trial_data));
    for (unsigned i = 0; i < thrd_n; ++i) {
        data[i].map         =   &map;
        data[i].seed        =   seed++;
        data[i].max_attempt =   max_attempt;
        data[i].buckets     =   buckets;
        data[i].stride      =   thrd_n;
    }

    // attempt header
    attempt_header_prnt_(group->len, buckets, max_attempt);

    // spawn threads
    pthread_t   *const  threads =   s_malloc(thrd_n * sizeof(pthread_t));
    bool        *const  live    =   s_calloc(thrd_n, sizeof(bool));
    for (unsigned i = 1; i < thrd_n; ++i) {
        if (pthread_create(&threads[i], nullptr, trial_multi_, &data[i])) {
            // thread failure
            fprintf(stderr, "\r\x1b[2Kskipping seed $%u + %un, n \\in \\mathbb{Z}_{\\geq 0}$\n", i, thrd_n);
            continue;
        }
        live[i]     =   true;
    }

    // local thread
    trial_multi_(&data[0]);

    // terminate
    for (unsigned i = 1; i < thrd_n; ++i) {
        if (live[i])                            pthread_join(threads[i], nullptr);
    }
    free(live);

    // end
    free_str_map_(&map);
    free(data);
    free(threads);
    fputc('\n', stdout);
    return  (hash_fn){ .offset=offset_t, .buckets=buckets, .seed=seed_t };
}

#endif  /* NTHREAD */

/*-PERFECT-HASHING-ALGORITHM-FULL-CALL--------------------------------------------------------------------------------*/

/**
 * Perfect hashing algorithm.
 *
 * @param       group           hash group
 * @param       seed            initial seed offset
 * @param       max_attempt     maximum attempt before seed rotation
 * @param       offset          initial offset
 */
[[nodiscard]] hash_fn algorithm( const hash_grp *const group,
                                       uint64_t        seed,
                                 const size_t          max_attempt,
                                       size_t          offset       ) {         // perfect hash algorithm
    // offset check
    if (offset >= 64) {
        fprintf(stderr, "OOB offset\n");
        exit(1);
    }

#ifndef NTHREAD
    return  alg_orch_(group, seed, max_attempt, offset);
#else
    // general init
    nt_term             =   false;
    size_t      buckets =   group->size << offset;

    // string map init
    str_map     map     =   str_map_make_(group);

    // attempts
    uint64_t                offs    =   0;
    attempt_header_prnt_(group->len, buckets, max_attempt);
    while (!nt_term)        offs    =   trial_(map, seed++, max_attempt, buckets);

    // end
    free_str_map_(&map);
    fputc('\n', stdout);
    return  (hash_fn){ .offset=offs, .buckets=buckets, .seed=seed - 1 };
#endif  /* NTHREAD */
}
