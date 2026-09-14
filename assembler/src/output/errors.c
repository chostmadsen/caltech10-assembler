/**
 * src/output/errors.c
 * Error output and exiting functions.
 */

#include    <stdlib.h>
#include    <stdio.h>
#include    <pthread.h>

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"

/*-EXITING-FUNCTIONS--------------------------------------------------------------------------------------------------*/

/**
 * Error number counter.
 */
int                     err_num             =   0;                              // number of errors

/**
 * Emits exit code from the assembler. Separately callable for successful exits (exit(0)) for debugging tools.
 * The io-mutex is not locked here; either ensure thread safety or manually lock the io mutex.
 *
 * @param       code            exit code
 */
void cit10a_exit_msg(const unsigned code) {                                     // assembler exit message
    // exit message
    if      (code == 0)               fputs(ASSEMBLER_HEAD CLR_OK     "exit code", stdout);
    else if (code < EX_EXT_THRS)      fputs(ASSEMBLER_HEAD CLR_ERR    "exit code", stderr);
    else                            { fputs(ASSEMBLER_HEAD CLR_INTRNL "exit code", stderr); return; }

    // exit code
    FILE   *stream;
    if (code == 0) {
        stream  =   stdout;
        fprintf(stdout, " %u\x1b[0m", code);
    } else {
        stream  =   stderr;
        fprintf( stderr, " %u\x1b[0m " CLR_DIM "[ %d error%s generated",
                 code, err_num, (err_num != 1) ? "s" : ""                );
    }

    if (warn_num > 0) {
        // warning output
        if    (code == 0)   fputs(" " CLR_DIM "[ ", stdout);
        else                fputs(" | ", stderr);
        fprintf( stream, CLR_DIM "%d warning%s generated ]\x1b[0m",
                 warn_num, (warn_num != 0) ? "s" : ""                 );
    }

    // ender
    if (warn_num <= 0 && code != 0)     fputs(" ]\x1b[0m", stderr);
    fputc('\n', stream);
}

/**
 * Exits from the assembler with an exit code. Locks the io mutex to exit on call.
 *
 * @param       code            exit code
 */
[[noreturn]] void cit10a_exit(const unsigned code) {                            // assembler exit
    // io mutex perma-lock
    pthread_mutex_lock(&io_mutex);

    // message and exit
    cit10a_exit_msg(code);
    exit((int)code);
}
