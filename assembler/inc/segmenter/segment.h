/**
 * inc/segmenter/segment.h
 * documentation @ src/segmenter/segment.c
 */

#pragma     once

#include    "helpers/general.h"
#include    "output/messages.h"
#include    "datastructures/stackmap.h"

/*-VAR-TOKEN----------------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // var token stackmap head
            smap_head   head;
            size_t      ln,     col;
} var_tok;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] smap_head get_identifier(strptr *sptr, rprt_f *err_f);            // identifier getter

[[nodiscard]] int parse_org(strptr *sptr, rprt_f *err_f);                       // .org parser
[[nodiscard]] bool verify_sctn_strt(strptr *sptr, rprt_f *err_f);               // .section start verification

[[nodiscard]] bool identifier_verify(       stackmap *smap,
                                      const void     *smap_itm, 
                                            rprt_f   *err_f     );              // identifier verification

void print_var_tok(const var_tok *var);                                         // var_tok printer
