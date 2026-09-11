/**
 * inc/output/external.h
 * documentation @ src/output/external.c
 */

#pragma     once

#include    <stdio.h>
#include    <pthread.h>

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#define                 ASSEMBLER_NAME          "cit10a"                        // assembler name
#define                 OUT_INDENT              "       "                       // output indent size
#define                 ASSEMBLER_INDENT              " "                       // assembler output indent size
#define                 ASSEMBLER_VERSION       "0.1.9"                         // assembler version
#define                 ASSEMBLER_DATE          "2026.09.04"                    // assembler release date

#define                 CLR_BOLD                "\x1b[1m"                       // bold color code
#define                 CLR_DIM                 "\x1b[2m"                       // dim color code
#define                 CLR_UNDRLN              "\x1b[4m"                       // underline color code
#define                 CLR_UNDRLN_OFF          "\x1b[24m"                      // underline color off code
#define                 CLR_INVERT              "\x1b[7m"                       // invert color code
#define                 CLR_OK                  "\x1b[32m"                      // ok message color code
#define                 CLR_NORM                "\x1b[34m"                      // normal message color code
#define                 CLR_VRBSE               "\x1b[36m"                      // verbose message color code
#define                 CLR_WARN                "\x1b[33m"                      // warning message color code
#define                 CLR_ERR                 "\x1b[31m"                      // error message color code
#define                 CLR_INTRNL              "\x1b[35m"                      // internal error message color code
#define                 CLR_INTRNL_WRN          "\x1b[2m\x1b[35m"               // internal warning message color code

#define                 MSG_NORM                "note"                          // message headers
#define                 MSG_VRBSE               "info"
#define                 MSG_WARN                "warning"
#define                 MSG_ERR                 "error"
#define                 MSG_INTRNL              "internal"
#define                 MSG_INTRNL_WRN          "internal warning"
#define                 NULL_CHR                CLR_ERR CLR_BOLD CLR_INVERT                                            \
                                                "0\x1b[0m"                      // null character
#define                 DEBUG_DELIM             CLR_VRBSE ">>\x1b[0m"           // debug delimiter

constexpr   int         DUMP_PRNT           =   4;                              // dump print verbosity threshold
constexpr   int         EXTRA_PRNT          =   3;                              // extra info print verbosity threshold
constexpr   int         INFO_PRNT           =   2;                              // info print verbosity threshold
constexpr   int         EXIT_PRNT           =   1;                              // exit print verbosity threshold

/*-COMBINED-CONSTANT-STRINGS------------------------------------------------------------------------------------------*/

#define                 MSG_NORM_CLR            CLR_NORM       MSG_NORM         // colored message headers
#define                 MSG_VRBSE_CLR           CLR_VRBSE      MSG_VRBSE
#define                 MSG_WARN_CLR            CLR_WARN       MSG_WARN
#define                 MSG_ERR_CLR             CLR_ERR        MSG_ERR
#define                 MSG_INTRNL_CLR          CLR_INTRNL     MSG_INTRNL
#define                 MSG_INTRNL_WRN_CLR      CLR_INTRNL_WRN MSG_INTRNL_WRN

#define                 ASSEMBLER_HEAD          "\x1b[0m" CLR_UNDRLN ASSEMBLER_NAME                                    \
                                                "\x1b[0m" ASSEMBLER_INDENT                                             \
                                                CLR_DIM "-\x1b[0m "             // assembler message head

/*-GLOBAL-MUTEXES-----------------------------------------------------------------------------------------------------*/

extern      pthread_mutex_t     io_mutex;                                       // input / output mutex

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void print_raw_str(const char *str, int n, FILE *stream);                       // raw string print

void cit10a_version_f(FILE *fp, bool ansi);                                     // assembler version dump
void cit10a_info(void);                                                         // assembler info message
void cit10a_startup(void);                                                      // assembler start message
