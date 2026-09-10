/**
 * src/datastructures/hash.c
 * Hashing implementation functions.
 */

#include    <stddef.h>
#include    <stdint.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "datastructures/hash.h"

/*-HASHING-FUNCTIONS--------------------------------------------------------------------------------------------------*/

/**
 * 64-bit FNV-1a hash.
 * 
 * @param       str             string to hash
 * @param       n               character number
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a(const char *str, size_t n) {                  // 64-bit fnv1a hash
    cit10a_asrt(str != nullptr);

    uint64_t    hash    =   FNV_OFFSET_64;
    while (n--) {
        hash    ^=  (uint64_t)*(str++);
        hash    *=  FNV_PRIME_64;
    }
    return  hash;
}

/**
 * 64-bit FNV-1a hash w/ a source slice.
 * 
 * @param       slice           slice to hash
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a_slc(const src_slice *const slice) {           // 64-bit fnv1a hash (slc)
    cit10a_asrt(slice != nullptr);
    return  hash_fnv1a(slice->str, slice->len);
}

/*-LOWERCASE-HASHING-FUNCTIONS----------------------------------------------------------------------------------------*/

/**
 * 64-biti FNV-1a hash (lower-cased).
 * 
 * @param       str             string to hash
 * @param       n               character number
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a_lwr(const char *str, size_t n) {              // lower 64-bit fnv1a hash
    cit10a_asrt(str != nullptr);

    uint64_t    hash    =   FNV_OFFSET_64;
    while (n--) {
        hash    ^=  (uint64_t)to_lwr_chr(*(str++));
        hash    *=  FNV_PRIME_64;
    }
    return  hash;
}

/**
 * 64-biti FNV-1a hash (lower-cased) w/ a source slice.
 * 
 * @param       slice           slice to hash
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a_slc_lwr(const src_slice *const slice) {       // lower 64-bit fnv1a hash (slc)
    cit10a_asrt(slice != nullptr);
    return  hash_fnv1a_lwr(slice->str, slice->len);
}

/*-FLAGGED-LOWERCASE-HASHING-FUNCTIONS--------------------------------------------------------------------------------*/

/**
 * 64-biti FNV-1a hash (lower-cased) with flag.
 * 
 * @param       str             string to hash
 * @param       n               character number
 * @param       flg             lowercase flag
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a_lwr_f( const char        *str, size_t n, 
                                               bool *const  flg            ) {  // flagged lower 64-bit fnv1a hash
    cit10a_asrt(str != nullptr);
    cit10a_asrt(flg != nullptr);

    uint64_t    hash        =   FNV_OFFSET_64;
    while (n--) {
        const   char    chr =   *(str++);
        *flg                =   (bool)((unsigned char)(chr - 'A') <= 25);
        hash                ^=  (uint64_t)to_lwr_chr(chr);
        hash                *=  FNV_PRIME_64;
    }
    return  hash;
}

/**
 * 64-biti FNV-1a hash (lower-cased) with flag w/ a source slice.
 * 
 * @param       slice           slice to hash
 * @param       flg             lowercase flag
 * @return                      hash
 */
[[nodiscard]] uint64_t hash_fnv1a_slc_lwr_f( const src_slice *const slice,       
                                                   bool      *const flg    ) {  // flagged lower 64-bit fnv1a hash (slc)
    cit10a_asrt(slice != nullptr);
    return  hash_fnv1a_lwr_f(slice->str, slice->len, flg);
}
