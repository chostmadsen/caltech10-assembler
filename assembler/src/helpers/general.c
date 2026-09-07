/**
 * src/helpers/general.c
 * General items used across various parts of the assembler.
 */

#include    <stdio.h>
#include    <string.h>
#include    <stdint.h>

#include    "output/external.h"
#include    "helpers/general.h"

/*-STRING-HELPERS-----------------------------------------------------------------------------------------------------*/

/**
 * Converts a character to lowercase.
 *
 * @param       chr                 character
 * @return                          lowercase
 */
[[nodiscard]] char to_lwr_chr(const char chr) {                                 // character to lower
    return  chr | (uint8_t)((chr - 'A') < 26) << 5;
}

/**
 * Whitespace character comparison; unrolled for performance.
 *
 * @param       chr             character to compare
 * @return                      whether chr is a whitespace character
 */
[[nodiscard]] bool is_whitespace(const char chr) {                              // whitespace characters
    return  ( chr == ' ' || chr == '\t' || chr == '\0' );
}
/*-STRING-SLICE-FUNCTIONS---------------------------------------------------------------------------------------------*/

/**
 * Prints a source slice to the specified stream.
 *
 * @param       slice           source slice
 * @param       stream          output stream
 */
void print_src_slice(const src_slice *const slice, FILE *const stream) {        // source slice printer
    print_raw_str(slice->str, slice->len, stream);
}

/**
 * Checks a source slice for equality.
 *
 * @param       slc_1           source slice 1
 * @param       slc_2           source slice 2
 * @return                      whether the slices are equal
 */
[[nodiscard]] bool srcslc_eq( const src_slice *const slc_1,
                              const src_slice *const slc_2  ) {                 // source slice equality check
    return  slc_1->len == slc_2->len && !memcmp(slc_1->str, slc_2->str, slc_1->len);
}

/**
 * Checks a source slice for equality. Lowercase variant.
 *
 * @param       slc_1           source slice 1
 * @param       slc_2           source slice 2
 * @return                      whether the slices are equal (case invariant)
 */
[[nodiscard]] bool srcslc_eq_lwr( const src_slice *const slc_1,
                                  const src_slice *const slc_2  ) {             // source slice equality check (lower)
    if (slc_1->len != slc_2->len)       return  false;
    for (size_t i = 0; i < slc_1->len; ++i) {
        if (to_lwr_chr(slc_1->str[i]) != to_lwr_chr(slc_2->str[i]))     return  false;
    }
    return  true;
}

/*-STRING-POINTER-FUNCTIONS-------------------------------------------------------------------------------------------*/

/**
 * Adjusts a string pointer by the given number.
 *
 * @param       ptr             string pointer
 * @param       n               number to adjust
 */
void adj_strptr(strptr *const ptr, const int n) {                               // string pointer adjust
    ptr->str    +=  n;
    ptr->col    +=  n;
}

/**
 * Increments a string pointer.
 *
 * @param       ptr             string pointer
 */
void inc_strptr(strptr *const ptr) {                                            // string pointer increment
    ++ptr->str;
    ++ptr->col;
}

/**
 * Decrements a string pointer.
 *
 * @param       ptr             string pointer
 */
void dec_strptr(strptr *const ptr) {                                            // string pointer decrement
    --ptr->str;
    --ptr->col;
}
