/**
 * src/output/external.c
 * Generalized output functions and output mutexes.
 */

#include    <stdio.h>
#include    <pthread.h>

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/external.h"
#include    "argparse/argparse.h"

/*-INPUT-/-OUTPUT-MUTEX-----------------------------------------------------------------------------------------------*/

/**
 * Output mutex to prevent input / output message interleaving.
 */
pthread_mutex_t             io_mutex        =   PTHREAD_MUTEX_INITIALIZER;      // input / output mutex

/*-RAW-STRING-PRINTER-------------------------------------------------------------------------------------------------*/

/**
 * Raw string print for `n` characters. Ensure `n` does not cause an OOB error.
 *
 * @param       str             string
 * @param       n               character number
 * @param       stream          output stream
 */
void print_raw_str(const char *str, int n, FILE *const stream) {
    acama_asrt(str != nullptr && stream != nullptr);

    while (n-- > 0) {
        const   char    chr =   *(str++);
        if   (chr != '\0')      fputc(chr, stream);
        else                    fputs(NULL_CHR, stream);
    }
}

/*-ASSEMBLER-GENERAL-EXTERNAL-OUTPUTS---------------------------------------------------------------------------------*/

/**
 * Prints the assembler version.
 */
static void acama_version(void) {
    fputs(CLR_UNDRLN ASSEMBLER_NAME "v" ASSEMBLER_VERSION, stdout);
    fprintf(stdout, CLR_UNDRLN_OFF "*");
#ifndef NDEBUG
    fputc('d', stdout);
#else
    fputc('-', stdout);
#endif  /* NDEBUG */
#ifndef NSANITIZE
    fputc('s', stdout);
#else
    fputc('-', stdout);
#endif  /* NDEBUG */
    fprintf(stdout, "%uB@" ASSEMBLER_DATE, CACHE_LN_S);
}

/**
 * Prints the assembler info.
 */
void acama_info(void) {                                                         // assembler info message
    acama_version();
    fprintf(stdout, CLR_DIM " [ %uB cache alignment ]" "\x1b[0m", CACHE_LN_S);
#ifndef NDEBUG
    fputs(" " CLR_INTRNL "DEBUG BUILD\x1b[0m", stdout);
#endif  /* NDEBUG */
#ifndef NSANITIZE 
    fputs(" " CLR_VRBSE "SANITIZED\x1b[0m", stdout);
#endif  /* NSANITIZE */
    fputc('\n', stdout);
}

/**
 * Prints assembler startup info (thread usage, files detected, structure, etc.)
 */
void acama_startup(void) {                                                      // assembler start message
    fputs(CLR_DIM, stdout);
    acama_version();
    fprintf( stdout, " | utilizing up to %d threads [ %d threads / %d file ]\x1b[0m\n",
                      c_args.n_thrds * c_args.n_files, c_args.n_thrds, c_args.n_files );
}
