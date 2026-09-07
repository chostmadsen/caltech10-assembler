/**
 * src/reader/reader_out.c
 * File struct printer functions.
 */

#include    <stdio.h>

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "reader/reader.h"
#include    "reader/reader_out.h"

/*-SOURCE-FILE-PRINTER------------------------------------------------------------------------------------------------*/

/**
 * Prints source file struct as a raw text stream, with line index items.
 *
 * @param       source_f        source file struct
 */
void print_src_f(const src_f *const source_f) {                                 // one-lined raw print full source file
    cit10a_asrt(source_f != nullptr);

    // info
    printf( DEBUG_DELIM CLR_DIM " [[ src_f %s::%zulns::%zuB ]]\x1b[0m\n",
            source_f->f_name, source_f->ln_num, source_f->size            );
    fputs(CLR_DIM "[0::0]\x1b[0m", stdout);

    // raw text stream
    size_t  ln_idx  =   1;
    for (size_t i = 0; i < source_f->size; ++i) {
        if      (source_f->text[i] != '\0')         fputc(source_f->text[i], stdout);
        else if (ln_idx < source_f->ln_num)       { printf( NULL_CHR CLR_DIM "[%zu::%zu]\x1b[0m",
                                                            ln_idx, source_f->ln_idxs[ln_idx]     ); 
                                                    ++ln_idx;                                        }
        else                                        fputs(NULL_CHR, stdout);
    }
    fputs("\n" DEBUG_DELIM "\n", stdout);
}

/*-SOURCE-FILE-INFO-PRINTER-------------------------------------------------------------------------------------------*/

/**
 * Prints source file struct info.
 *
 * @param       source_f        source file struct
 */
void print_src_f_info(const src_f *const source_f) {                            // print source file struct info
    cit10a_asrt(source_f != nullptr);
    cit10a_msg( &(msg_info){ .type=msg_vrbse_t, .header="source file read",
                             .report_f=&(rprt_f){ .file=source_f, .len=0 } },
                             "%zu lines, %zu characters", source_f->ln_num, source_f->len );
}
