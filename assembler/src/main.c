/**
 * src/main.c
 * Main assembler call.
 */

// windows check
#if     defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define     NTHREAD     // if this is ever not an error, make sure this is defined
#error      "cit10a does not support windows"
#endif

// c version checks
#if     !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
// this will get buried under a large amount of other compiler warnings, but it is here
#error      "cit10a requires c23+ (-std=c23)"
#endif  /* !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L */

// threading checks
#ifndef NTHREAD
#ifdef  __STDC_NO_ATOMICS__
#warning    "cit10a requires atomics if threading is not disabled via -DNTHREAD"
#define     NTHREAD
#endif  /* __STDC_NO_ATOMICS__ */
#if     !__has_include(<pthread.h>)
#warning    "cit10a requires posix threads if threading is not disabled via -DNTHREAD"
#define     NTHREAD
#endif  /* !__has_include(<pthread.h>) */
#endif  /* NTHREAD */

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "reader/reader_out.h"
#include    "preprocessor/preprocessor.h"
#include    "preprocessor/folder_parse.h"
#include    "preprocessor/folder_parse_out.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/assemble.h"
#include    "assemble/assemble_emit.h"

/*-MAIN---------------------------------------------------------------------------------------------------------------*/

/**
 * Main assembler call.
 *
 * @return                      exit code
 */
int main(const int argc, const char *const *const argv) {                       // main
    // get compilation flags
    if (parse_args(argc, argv)) {
        free_c_args();
        return  0;
    }
    if (c_args.verbosity >= EXTRA_PRNT)     cit10a_startup();

    // read source file
    src_f       source_f        =   read_source(c_args.target);
    // source file output
    if (c_args.verbosity >= INFO_PRNT)                          print_src_f_info(&source_f);
    if (c_args.verbosity >= DUMP_PRNT || c_args.emit.file)      print_src_f(&source_f);

    // sources get (make sure inc is first; otherwise, change in folder_parse.c, and 2 for the 2 sources)
    sources     srcs            =   get_sources(2, &c_args.inc, &c_args.src);
    // preprocessor
    preprocess(&source_f, &srcs);
    // sources output
    if (c_args.verbosity >= INFO_PRNT)                          print_sources_info(&srcs);
    if (c_args.verbosity >= DUMP_PRNT || c_args.emit.files)     print_sources(&srcs);
    if (c_args.verbosity >= DUMP_PRNT)                          print_sources_dump(&srcs);

    // segment processor
    segmaps     smaps           =   segment(&source_f, &srcs);
    // segmenter output
    if (c_args.verbosity >= INFO_PRNT)                          print_segmap_info(&smaps);
    if (c_args.verbosity >= DUMP_PRNT || c_args.emit.table)     print_segmap(&smaps);

    // assembly
    asm_ret     asm_r           =   assemble(&smaps);
    // assembly output
    if (c_args.verbosity >= INFO_PRNT)                          print_asm_info(&asm_r);
    if (c_args.verbosity >= DUMP_PRNT || c_args.emit.aasm)      print_asm(&asm_r);


    if (werror_exit)            cit10a_exit(WARN_ERRNO);
    // emit assembly
    emit_asm(c_args.output, &source_f, &srcs, &smaps, &asm_r);
    if (c_args.verbosity >= INFO_PRNT)                          print_asm_emit_info(c_args.output);

    // end free
    free_src_f(&source_f);
    free_sources(&srcs);
    free_segmap(&smaps);
    free_asm_ret(&asm_r);

    // assembler exit
    cit10a_exit_msg(0);
    free_c_args();
    return  0;
}
