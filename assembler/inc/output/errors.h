/**
 * inc/output/errors.h
 * documentation @ src/output/errors.c
 */

#pragma     once

/*-EXIT-ERROR-NUMBERS-------------------------------------------------------------------------------------------------*/

// user error codes
constexpr   unsigned    EXCESS_ERRNO        =   1;                              // too many errors exit
constexpr   unsigned    WARN_ERRNO          =   2;                              // -Werror exit
constexpr   unsigned    ARGPARSE_ERRNO      =   3;                              // argument parser failure exit
constexpr   unsigned    READER_ERRNO        =   4;                              // reader failure exit

constexpr   unsigned    EX_EXT_THRS         =   100;                            // internal exit threshold
// internal error codes
constexpr   unsigned    INTRNL_ERRNO        =   101;                            // unspecified internal exit
constexpr   unsigned    ALLOC_ERRNO         =   102;                            // alloc exit

/*-GLOBAL-ERROR-COUNTER-----------------------------------------------------------------------------------------------*/

extern      int         err_num;                                                // number of errors

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void acama_exit_msg(unsigned code);                                             // assembler exit message
[[noreturn]] void acama_exit(unsigned code);                                    // assembler exit

/*-MACROS-------------------------------------------------------------------------------------------------------------*/

/**
 * Assertions with formatted output (expanded macro).
 * No io mutex lock; abort should be immediate, and can occur within message printing. The mutex may cause a deadlock,
 * which must be maximally avoided on abort calls.
 *
 * @param       expr            assertion expression
 */
#ifndef NDEBUG
#include    <stdlib.h>
#include    <stdio.h>

#include    "output/external.h"

#define _STR_INDR_(x)   #x
#define _STR_(x)        _STR_INDR_(x)

#define acama_asrt(expr) (                                                                                             \
            (expr) ? (void)0 : (                                                                                       \
                fputs( "\r\x1b[2K" ASSEMBLER_HEAD CLR_INTRNL "failed assertion"                                        \
                       "\x1b[0m [ " __FILE__ "::"_STR_(__LINE__) " ] : " CLR_DIM #expr "\x1b[0m\n", stderr ),          \
                abort()                                                                                                \
            )                                                                                                          \
        )

#else
#define acama_asrt(expr)        ((void)0)
#endif  /* NDEBUG */
