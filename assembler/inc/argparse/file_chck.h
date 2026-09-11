/**
 * inc/argparse/file_chck.h
 * documentation @ src/argparse/file_chck.c
 */

#pragma     once

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   char        FILE_EXTNS_CHR      =   '.';                            // file extension character

constexpr   char        ALLOWED_EXTNS[][4]  =   { "asm",    "a" };              // allowed file extensions
constexpr   char        ALLOWED_INC[][4]    =   { "inc" };                      // allowed .inc file extensions
 
#define                 OBJ_EXTENSION           ".obj"                          // obj file extension (default out)

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] const char *file_extension(const char *f_nm);                     // gets file extension
[[nodiscard]] bool verify_target(const char *target);                           // verify target extension
[[nodiscard]] char *get_output(const char *target, const char *output);         // get output
