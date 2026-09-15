/**
 * src/output/messages.c
 * Assembler messaging functions.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <stdarg.h>

#ifndef NTHREAD
#include    <pthread.h>
#endif  /* NTHREAD */

#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"

/*-INTERNAL-STRING-FORMATTER-FUNCTIONS--------------------------------------------------------------------------------*/

/**
 * Prints the start of a assembler message to the given file stream.
 * Does not lock the io mutex; it is assumed that the caller will have already handled this.
 *
 * @param       stream          output stream
 * @param       indents         number of indents
 */
static void cit10a_msg_start_(FILE *const stream, const size_t indents) {       // assembler message start printer
    fprintf(stream, "\x1b[0m" OUT_INDENT MSG_DELIM "\x1b[0m%*s", (int)indents, "");
}

/*-EXTERNAL-LOGGING-AND-PRINTING-FUNCTIONS----------------------------------------------------------------------------*/

/**
 * Pre-allocate space for this;
 * only modified within a mutex lock (thread safe), and always allocated, so no failure on stack overflows or OOM.
 */
static  char            msg_buf[MAX_MSG_S];                                     // output message buffer

/**
 * -Werror exit flag and warning counter.
 */
int                     warn_num            =   0;                              // warning number
bool                    werror_exit         =   false;                          // -Werror exit
bool                    warn_num_msg        =   false;                          // warning number supression msg

/**
 * Emits a log message to stdout (if info.type is normal) or stderr (if info.type is non-normal).
 * Locks the message call w/ a mutex to prevent error message interleaving.
 * Start va_list externally.
 *
 * @param       info            message info { .type, .header, .report_f }
 * @param       fmt             message format
 * @param       args            variadic args
 */
void cit10a_msg_v( const msg_info *const info, 
                   const char     *const fmt,
                   const va_list  *const args  ) {                              // assembler message emitter (va)
    // assembler message emitter
    cit10a_asrt(info != nullptr);

    // warning message suppression
    if (c_args.warnings.off && info->type == msg_warn_t)            return;
    if (warn_num_msg && info->type == msg_warn_t)                   return;

#ifndef NTHREAD
    // mutex lock
    pthread_mutex_lock(&io_mutex);
#endif  /* NTHREAD */

    // -w overflow
    if (warn_num >= c_args.max_warns && info->type == msg_warn_t) {
        fprintf( stderr, MSG_WARN_CLR "\x1b[0m : too many warnings; " "omitting further warnings [ -w%d ]\n",
                 c_args.max_warns                                                                             );
        warn_num_msg    =   true;
        goto    msg_end;
    }

    // output stream and message start
    FILE       *stream;
    if   (info->type == msg_norm_t || info->type == msg_vrbse_t)    stream  =   stdout;
    else                                                            stream  =   stderr;
    fputs(ASSEMBLER_HEAD, stream);

    // output stream and message color
    switch (info->type) {
        case msg_norm_t:
            fputs(MSG_NORM_CLR "\x1b[0m", stream);
            break;
        case msg_vrbse_t:
            fputs(MSG_VRBSE_CLR "\x1b[0m", stream);
            break;
        case msg_warn_t:
            // warning to error
            if (c_args.warnings.err) {
                fputs(MSG_ERR_CLR "\x1b[0m", stream);
                werror_exit     =   true;
                ++err_num;
            }
            else {
                fputs(MSG_WARN_CLR "\x1b[0m", stream);
                ++warn_num;
            }
            break;
        case msg_err_t:
            ++err_num;
            fputs(MSG_ERR_CLR "\x1b[0m", stream);
            break;
        case msg_intrnl_t:
            fputs(MSG_INTRNL_CLR "\x1b[0m", stream);
            break;
        case msg_intrnl_wrn_t:
            fputs(MSG_INTRNL_WRN_CLR "\x1b[0m", stream);
            break;
        default:
            cit10a_asrt(!"message type oob");
            cit10a_exit_msg(INTRNL_ERRNO);
            exit((int)INTRNL_ERRNO);
    }

    // message
    size_t  indent_len      =   0;
    if (info->report_f != nullptr) {
        // file info
        cit10a_asrt(info->report_f->file != nullptr);
        const   rprt_f  *const  report_f    =   info->report_f;
        const   src_f   *const  file        =   report_f->file;
        cit10a_asrt(file->f_name != nullptr);
        fprintf(stream, " [ %s", file->f_name);

        if (report_f->len != 0) {
            // line information
            fprintf(stream, "::%zu:%zu", report_f->ln + 1, report_f->col + 1);
        }

        // message header
        fprintf(stream, " ] : %s\x1b[0m\n", info->header);

        if (report_f->len != 0) {
            // source file line
            fprintf( stream, CLR_DIM "%*zu\x1b[0m " MSG_DELIM "\x1b[0m%s\n",
                             (int)sizeof(OUT_INDENT) - 2, report_f->ln + 1, src_f_getline(file, report_f->ln) );

            // set indent length and put `^` / `~` characters
            indent_len      =   report_f->col;
            cit10a_msg_start_(stream, indent_len);
            fputs(CLR_DIM "^", stream);
            for (size_t i = 1; i < report_f->len; ++i) {
                fputc('~', stream);
            }
            fputs("\x1b[0m ", stream);
            indent_len      +=  report_f->len + 1;
        } else {
            // message start
            cit10a_msg_start_(stream, 0);
        }
    } else {
        // message header and start
        fprintf(stream, " : %s\x1b[0m\n", info->header);
        cit10a_msg_start_(stream, 0);
    }

    // no main message content
    if (fmt == nullptr) {
        fputs("\r\x1b[2K", stream);
        goto    msg_end;
    }

    // message buffer copy
    const   int         msg_len     =   vsnprintf(msg_buf, MAX_MSG_S, fmt, *args);
    cit10a_asrt(msg_len >= 0);
    const   size_t      msg_end_c   =   ((unsigned)msg_len >= MAX_MSG_S) ? MAX_MSG_S - 1 : (size_t)msg_len;
    msg_buf[msg_end_c]              =   '\0';

    // print message
    const   char   *ln_strt         =   msg_buf;
    for (const char *ptr = msg_buf; ; ++ptr) {
        // normal character skip
        if (*ptr != '\n' && *ptr != '\0')       continue;

        // fwrite
        fwrite(ln_strt, sizeof(char), (size_t)(ptr - ln_strt), stream);
        if (*ptr == '\0')                       break;

        fputc('\n', stream);
        cit10a_msg_start_(stream, indent_len);
        ln_strt     =   ptr + 1;
    }

    // variadic end
    fputs("\n", stream);

msg_end:
    if (err_num >= c_args.max_errs) {
        // error count overflow
        fprintf(stderr, ASSEMBLER_HEAD MSG_ERR_CLR "\x1b[0m : too many errors [ -e%d ]\n", c_args.max_errs);
        cit10a_exit_msg(EXCESS_ERRNO);
        exit((int)EXCESS_ERRNO);
    }

#ifndef NTHREAD
    // mutex unlock
    pthread_mutex_unlock(&io_mutex);
#endif  /* NTHREAD */
}

/**
 * Emits a log message to stdout (if info.type is normal) or stderr (if info.type is non-normal).
 * Locks the message call w/ a mutex to prevent error message interleaving.
 *
 * @param       info            message info { .type, .header, .report_f }
 * @param       fmt             message format
 * @param       ...             variadic args
 */
void cit10a_msg(const msg_info *const info, const char *const fmt, ...) {       // assembler message emitter
    if (fmt == nullptr) {
        // nullptr handle
        cit10a_msg_v(info, nullptr, nullptr);
        return;
    }
    // va start
    va_list     args;
    va_start(args, fmt);
    cit10a_msg_v(info, fmt, &args);
    va_end(args);
}
