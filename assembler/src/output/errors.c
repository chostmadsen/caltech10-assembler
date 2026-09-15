/**
 * src/output/errors.c
 * Error output and exiting functions.
 */

#include    <stdlib.h>
#include    <stdio.h>

#ifndef NTHREAD
#include    <pthread.h>
#endif  /* NTHREAD */

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/argparse.h"

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
    if (code >= EX_EXT_THRS) {
        // critical error (do not ignore)
        fprintf(stderr, CLR_INTRNL "exit code %u", code);
        return;
    }

    // exit message
    FILE   *stream  =   (code == 0) ? stdout : stderr;
    if (c_args.verbosity >= EXIT_PRNT) {
        if   (code == 0)    fprintf(stream, CLR_OK  "assembled at \x1b[0m%s",  c_args.output);
        else                fprintf(stream, CLR_ERR "exit code %u\x1b[0m ", code);
    }

    // exit code
    if (code != 0) {
        fprintf(stream, CLR_DIM "[ %d error%s generated", err_num, (err_num != 1) ? "s" : "");
    }

    if (warn_num > 0) {
        // warning output
        if (code == 0 && c_args.verbosity >= EXIT_PRNT)     fputc(' ', stream);

        if    (code == 0)   fputs(CLR_DIM "[ ", stream);
        else                fputs(" | ", stream);
        fprintf( stream, CLR_DIM "%d warning%s generated ]\x1b[0m",
                 warn_num, (warn_num != 1) ? "s" : ""               );
    }

    // ender
    if (warn_num <= 0 && code != 0)                                     fputs(" ]\x1b[0m", stderr);
    if (c_args.verbosity >= EXIT_PRNT || warn_num > 0 || code != 0)     fputc('\n', stream);
}

/**
 * Exits from the assembler with an exit code. Locks the io mutex to exit on call.
 *
 * @param       code            exit code
 */
[[noreturn]] void cit10a_exit(const unsigned code) {                            // assembler exit
#ifndef NTHREAD
    // io mutex perma-lock
    pthread_mutex_lock(&io_mutex);
#endif  /* NTHREAD */

    // message and exit
    cit10a_exit_msg(code);
    exit((int)code);
}
