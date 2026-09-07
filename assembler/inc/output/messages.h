/**
 * inc/output/messages.h
 * documentation @ src/output/messages.c
 */

#pragma     once

#include    <stdlib.h>
#include    <stdarg.h>

#include    "reader/reader.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

#define                 MSG_DELIM               "\x1b[2m| "                     // message delimiter string
constexpr   unsigned    MAX_MSG_S           =   2048;                           // maximum error message size
                                                                                // truncation is better than oom here
/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // file error reporter struct
    const   src_f      *file;
            size_t      ln,     col,    len;
} rprt_f;

typedef enum {                                                                  // message info struct type
    msg_norm_t,     msg_vrbse_t,    msg_warn_t,     msg_err_t,      msg_intrnl_t,   msg_intrnl_wrn_t
} msg_info_t;
typedef struct {                                                                // message info struct
    const   msg_info_t  type;
    const   char       *header;
    const   rprt_f     *report_f;
} msg_info;

/*-GLOBAL-WERROR-EXIT-FLAG--------------------------------------------------------------------------------------------*/

extern      bool        werror_exit;                                            // -Werror exit

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void cit10a_msg_v(const msg_info *info, const char *fmt, const va_list *args);  // assembler message emitter (va)
void cit10a_msg(const msg_info *info, const char *fmt, ...);                    // assembler message emitter
void find_file_loc(rprt_f *file, size_t idx);                                   // character locator
