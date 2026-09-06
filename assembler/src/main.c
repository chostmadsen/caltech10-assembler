/**
 * src/main.c
 * Main assembler call.
 */

#include    <unistd.h>
#ifdef  __STDC_NO_ATOMICS__
#error  "acama requires atomics"
#endif
#if     !defined(_POSIX_THREADS) || (_POSIX_THREADS <= 0)
#error  "acama requires posix threads"
#endif

#include    "output/errors.h"
#include    "output/external.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "reader/reader_out.h"

/*-MAIN---------------------------------------------------------------------------------------------------------------*/

/**
 * Main assembler call.
 *
 * @return                      exit code
 */
int main(const int argc, const char *const *const argv) {                       // main
    // get compilation flags
    if (parse_args(argc, argv))     goto    comp_exit;
    if (c_args.verbosity >= 3)      acama_startup();

    // read source file
    src_f       source_f        =   read_source(c_args.target);  // TODO : this will fuck me later
    // source file output
    if (c_args.verbosity >= 3)                          print_src_f_info(&source_f);
    if (c_args.verbosity >= 4 || c_args.emit.file)      print_src_f(&source_f);

    // TODO : preprocessor

    // segment processor

    // assembly

comp_exit:
    // assembler exit
    if (c_args.verbosity >= 1)      acama_exit_msg(0);
    return  0;
}

