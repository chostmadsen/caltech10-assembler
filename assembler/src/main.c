/**
 * src/main.c
 * Main assembler call.
 */

#include    <unistd.h>
#ifdef  __STDC_NO_ATOMICS__
#error  "cit10a requires atomics"
#endif
#if     !defined(_POSIX_THREADS) || (_POSIX_THREADS <= 0)
#error  "cit10a requires posix threads"
#endif

#include    "output/errors.h"
#include    "output/external.h"
#include    "datastructures/stackmap.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "reader/reader_out.h"
#include    "segmenter/constseg.h"
#include    "segmenter/dataseg.h"

/*-MAIN---------------------------------------------------------------------------------------------------------------*/

/**
 * Main assembler call.
 *
 * @return                      exit code
 */
int main(const int argc, const char *const *const argv) {                       // main
    // get compilation flags
    if (parse_args(argc, argv))     goto    comp_exit;
    if (c_args.verbosity >= 3)      cit10a_startup();

    // read source file
    src_f       source_f        =   read_source(c_args.target);
    // source file output
    if (c_args.verbosity >= 3)                          print_src_f_info(&source_f);
    if (c_args.verbosity >= 4 || c_args.emit.file)      print_src_f(&source_f);

    // TODO : preprocessor

    // segment processor
    bool        seg_err         =   false;
    stackmap    const_smap      =   constseg(&source_f, &seg_err);
    print_const_map(&const_smap);
    seg_err                     =   false;
    stackmap    data_smap       =   dataseg(&source_f, &seg_err);
    print_data_map(&data_smap);

    // assembly

comp_exit:
    // assembler exit
    if (c_args.verbosity >= 1)      cit10a_exit_msg(0);
    return  0;
}
