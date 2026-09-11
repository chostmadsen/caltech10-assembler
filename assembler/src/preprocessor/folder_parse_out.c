/**
 * src/preprocessor/folder_parse_out.c
 * Folder parser (source reader) output printers.
 */

#include    <stddef.h>
#include    <stdio.h>

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stackmap.h"
#include    "preprocessor/folder_parse.h"
#include    "preprocessor/folder_parse_out.h"

/*-PRINTER-HELPERS----------------------------------------------------------------------------------------------------*/

/**
 * Prints all sources found.
 *
 * @param       f_itm_v         file name stackmap item
 */
static void fname_itm_prnt_(const void *const f_itm_v) {                        // filename struct print
    cit10a_asrt(f_itm_v != nullptr);

    const   fname_itm   *const  f_itm   =   (fname_itm*)f_itm_v;
    print_smap_head(f_itm);
    printf(" [ dir=%s ]", f_itm->folder);
}

/**
 * Prints a cached file (file that has been read).
 *
 * @param       f_itm_v         src_f stackmap item
 */
static void fcache_itm_prnt_(const void *const f_itm_v) {                       // cached file struct print
    cit10a_asrt(f_itm_v != nullptr);

    const   src_f_sm    *const  f_itm   =   (src_f_sm*)f_itm_v;
    print_smap_head(f_itm);
    printf(" [ %zuB ]", f_itm->source->size);
}

/*-PRINTERS-----------------------------------------------------------------------------------------------------------*/

/**
 * Sources dump (files and cached files together).
 *
 * @param       srcs            sources
 */
void print_sources(const sources *const srcs) {                                 // prints file sources
    cit10a_asrt(srcs != nullptr);

    fputs(DEBUG_DELIM CLR_DIM " [[ sources ]]\n", stdout);
    for (size_t i = 0; i < srcs->num; ++i) {
        printf(CLR_DIM "-%c files\x1b[0m\n", srcs->items[i].flag);
        if (srcs->items[i].empty) {
            fputs("(empty)\x1b[0m\n", stdout);
            continue;
        }
        fputs(CLR_DIM "found files ", stdout);
        print_stackmap(&srcs->items[i].files, fname_itm_prnt_);
        fputs(CLR_DIM "cached files ", stdout);
        print_stackmap(&srcs->items[i].open_files, fcache_itm_prnt_);
    }
    fputs(DEBUG_DELIM "\n", stdout);
}

/**
 * Sources information.
 *
 * @param       srcs            sources
 */
void print_sources_info(const sources *const srcs) {                            // print file sources information
    cit10a_asrt(srcs != nullptr);
    cit10a_msg( &(msg_info){ .type=msg_vrbse_t, .header="sources found", .report_f=nullptr }, "-%c %s, -%c %s",
                             srcs->items[0].flag, (srcs->items[0].empty) ? "skipped" : "tracked",
                             srcs->items[1].flag, (srcs->items[1].empty) ? "skipped" : "tracked"                );

}
