/**
 * src/argparse/file_chck.c
 * File checkers from argparser.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <string.h>

#include    "helpers/general.h"
#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "argparse/file_chck.h"

/*-FILE-CHECKERS------------------------------------------------------------------------------------------------------*/

[[nodiscard]] const char *file_extnsn_( const char *const f_nm,
                                        const int         f_len ) {             // get file extension (length provided)
    cit10a_asrt(f_nm != nullptr);
    for (int i = f_len - 1; i > 0; --i) {
        // valid file extension
        if (f_nm[i] == FILE_EXTNS_CHR) {
            if    (i != f_len)              return  f_nm + i + 1;
            else                            return  nullptr;
        }
    }
    return  nullptr;
}

/**
 * Get the file extension of a string. nullptr for no extension.
 * 
 * @param       f_nm            file name
 * @return                      pointer to extension
 */
[[nodiscard]] const char *file_extension(const char *const f_nm) {              // gets file extension
    cit10a_asrt(f_nm != nullptr);
    return  file_extnsn_(f_nm, strlen(f_nm));
}

/**
 * Get the file extension of a string. empty src_slice for no extension.
 * 
 * @param       f_nm            file name
 * @return                      pointer to extension
 */
[[nodiscard]] src_slice file_extension_slc(const src_slice *const slc) {        // gets file extension
    cit10a_asrt(slc != nullptr);
    const   char    *const  extnsn  =   file_extnsn_(slc->str, slc->len);
    if (extnsn == nullptr)              return  (src_slice){ .str=nullptr, .len=0 };
    return  (src_slice){ .str=extnsn, .len=slc->len + (slc->str - extnsn)};
}

/**
 * Verifies the target extension.
 *
 * @param       target          target file name
 * @return                      whether target is valid
 */
[[nodiscard]] bool verify_target(const char *const target) {                    // verify target extension
    // get extension
    const   char    *const  f_extns             =   file_extension(target);
    if (f_extns == nullptr)                         return  false;

    for (size_t i = 0; i < arr_s(SRC_EXTNS); ++i) {
        // check array
        if (!strcmp(f_extns, SRC_EXTNS[i]))         return  true;
    }
    return  false;
}

/**
 * Generate the output file, creating a new file to target if none is provided.
 *
 * @param       target          target
 * @param       output          provided output (or nullptr)
 * @return                      output name
 */
[[nodiscard]] char *get_output( const char *const target,
                                const char *const output  ) {                   // get output
    if (output != nullptr) {
        // keep output formatting consistant
        const   size_t  n_itms  =   strlen(output) + 1;
        char    *const  outp    =   chckd_malloc(n_itms * sizeof(char), "output char*");
        memcpy(outp, output, n_itms);
        return  outp;
    }

    cit10a_asrt(target != nullptr);
    // create output
    const   int     f_targ_len  =   file_extension(target) - target - 1;
    const   int     f_targ_tot  =   f_targ_len + sizeof(OBJ_EXTENSION);
    char    *const  outp        =   chckd_malloc(f_targ_tot * sizeof(char), "output char*");
    memcpy(outp, target, f_targ_len);
    memcpy(outp + f_targ_len, OBJ_EXTENSION, sizeof(OBJ_EXTENSION));
    return  outp;
}
