/**
 * inc/argparse/file_chck.h
 * documentation @ src/argparse/file_chck.c
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   char        FILE_EXTNS_CHR      =   '.';                            // file extension character

constexpr   size_t      ECL                 =   4;                              // extension length
constexpr   char        SRC_FLG             =   'S';                            // source flag
constexpr   char        SRC_EXTNS[][ECL]    =   { "asm",    "a" };              // allowed file extensions
constexpr   char        INC_FLG             =   'I';                            // include flag
constexpr   char        INC_EXTNS[][ECL]    =   { "inc" };                      // allowed .inc file extensions
 
#define                 OBJ_EXTENSION           ".obj"                          // obj file extension (default out)

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] const char *file_extension(const char *f_nm);                     // gets file extension
[[nodiscard]] src_slice file_extension_slc(src_slice *slc);                     // gets file extension
[[nodiscard]] bool verify_target(const char *target);                           // verify target extension
[[nodiscard]] char *get_output(const char *target, const char *output);         // get output
