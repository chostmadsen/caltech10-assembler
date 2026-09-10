/**
 * src/main.c
 * Main assembler call.
 */

#include "assemble/assemble_emit.h"
#if     defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#error  "cit10a does not support windows"
#endif

#include    <unistd.h>
#ifdef  __STDC_NO_ATOMICS__
#error  "cit10a requires atomics"
#endif
#if     !defined(_POSIX_THREADS) || (_POSIX_THREADS <= 0)
#error  "cit10a requires posix threads"
#endif

#include    "output/errors.h"
#include    "output/external.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "reader/reader_out.h"
#include    "preprocessor/preprocessor.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/assemble.h"

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
    if (c_args.verbosity >= 2)                          print_src_f_info(&source_f);
    if (c_args.verbosity >= 4 || c_args.emit.file)      print_src_f(&source_f);

    // preprocessor
    preprocess(&source_f);

    // segment processor
    segmaps     smaps           =   segment(&source_f);
    // segmenter output
    if (c_args.verbosity >= 2)                          print_segmap_info(&smaps);
    if (c_args.verbosity >= 4 || c_args.emit.table)     print_segmap(&smaps);

    // assembly
    asm_ret     asm_r           =   assemble(&smaps);
    // assembly output
    if (c_args.verbosity >= 2)                          print_asm_info(&asm_r);
    if (c_args.verbosity >= 4 || c_args.emit.aasm)      print_asm(&asm_r);


    // emit assembly
    emit_asm(stdout, &source_f, &smaps, &asm_r);

    // end free
    free_src_f(&source_f);
    free_segmap(&smaps);
    free_asm_ret(&asm_r);

comp_exit:
    // assembler exit
    if (c_args.verbosity >= 1)      cit10a_exit_msg(0);
    return  0;
}
