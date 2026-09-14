/**
 * src/output/external.c
 * Generalized output functions and output mutexes.
 */

#include    <stdio.h>

#ifndef NTHREAD
#include    <pthread.h>
#endif  /* NTHREAD */

#include    "helpers/general.h"
#include    "output/errors.h"
#include    "output/external.h"
#include    "argparse/argparse.h"

/*-INPUT-/-OUTPUT-MUTEX-----------------------------------------------------------------------------------------------*/

#ifndef NTHREAD
/**
 * Output mutex to prevent input / output message interleaving.
 */
pthread_mutex_t             io_mutex        =   PTHREAD_MUTEX_INITIALIZER;      // input / output mutex
#endif  /* NTHREAD */

/*-RAW-STRING-PRINTER-------------------------------------------------------------------------------------------------*/

/**
 * Raw string print for `n` characters. Ensure `n` does not cause an OOB error.
 *
 * @param       str             string
 * @param       n               character number
 * @param       stream          output stream
 */
void print_raw_str(const char *str, int n, FILE *const stream) {
    cit10a_asrt(str != nullptr && stream != nullptr);

    while (n-- > 0) {
        const   char    chr =   *(str++);
        if   (chr != '\0')      fputc(chr, stream);
        else                    fputs(NULL_CHR, stream);
    }
}

/*-ASSEMBLER-GENERAL-EXTERNAL-OUTPUTS---------------------------------------------------------------------------------*/

/**
 * Prints assembler version to an output.
 *
 * @param       fp              output
 * @param       ansi            ansi formatting
 */
void cit10a_version_f(FILE *const fp, const bool ansi) {                        // assembler version dump
    if (ansi)   fputs(CLR_UNDRLN, fp);
    fputs(ASSEMBLER_NAME "v" ASSEMBLER_VERSION, fp);

    if (ansi)   fputs(CLR_UNDRLN_OFF, fp);
    fputc('*', fp);
#ifndef NDEBUG
    fputc('a', fp);
#else
    fputc('-', fp);
#endif  /* NDEBUG */
#ifndef NSANITIZE
    fputc('s', fp);
#else
    fputc('-', fp);
#endif  /* NDEBUG */
#ifndef NTHREAD
    fputc('-', fp);
#else
    fputc('n', fp);
#endif  /* NTHREAD */
    fprintf(fp, "%uB@" ASSEMBLER_DATE, CACHE_LN_S);
}

/**
 * Prints the assembler info.
 */
void cit10a_info(void) {                                                        // assembler info message
    cit10a_version_f(stdout, true);
    fprintf(stdout, CLR_DIM " [ %uB cache alignment ]" "\x1b[0m", CACHE_LN_S);
#ifndef NDEBUG
    fputs(" " CLR_INTRNL "DEBUG BUILD\x1b[0m", stdout);
#endif  /* NDEBUG */
#ifndef NSANITIZE 
    fputs(" " CLR_VRBSE "SANITIZED\x1b[0m", stdout);
#endif  /* NSANITIZE */
#ifdef  NTHREAD
    fputs(" " CLR_DIM "NON-THREADED\x1b[0m", stdout);
#endif  /* NTHREAD */
    fputc('\n', stdout);
}

/**
 * Prints assembler startup info (thread usage, files detected, structure, etc.)
 */
void cit10a_startup(void) {                                                     // assembler start message
    fputs(CLR_DIM, stdout);
    cit10a_version_f(stdout, true);
    fprintf(stdout, " | utilizing up to %d threads\x1b[0m\n", c_args.n_thrds);
}
