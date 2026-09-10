/**
 * inc/segmenter/segment.h
 * documentation @ src/segmenter/segment.c
 */

#pragma     once

#include    <stddef.h>
#include    <limits.h>

#include    "helpers/general.h"
#include    "output/messages.h"
#include    "reader/reader.h"
#include    "datastructures/stackmap.h"

/*-CONSTANTS----------------------------------------------------------------------------------------------------------*/

constexpr   int         FLD_SHFT            =   sizeof(size_t) * CHAR_BIT;      // field shift
// NOTE : if this does not exist, change this to whatever power of 2 size_t is
constexpr   int         FLDS_PER            =   __builtin_ctz(sizeof(size_t)) + 3;

/*-STRUCTS-/-ENUMS----------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // var token stackmap head
            smap_head   head;
    const   src_f      *source;
            size_t      ln,     col;
} var_tok;

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

[[nodiscard]] smap_head get_identifier(strptr *sptr, rprt_f *err_f);            // identifier getter

[[nodiscard]] bool set_bitmap(size_t *bmap, int n_flds, int fld);               // bitset set

[[nodiscard]] int parse_org(strptr *sptr, rprt_f *err_f);                       // .org parser
[[nodiscard]] bool verify_sctn_strt(strptr *sptr, rprt_f *err_f);               // .section start verification
[[nodiscard]] int pseudo_hash_lu_adj(strptr *sptr);                             // .psuedo lookup w/ strptr adj

void range_msg( const var_tok   *var,
                      rprt_f    *err_f,
                      int        loc,
                      int        max,
                      msg_info_t msg_t );                                       // range check error message

[[nodiscard]] bool identifier_verify(       stackmap *smap,
                                      const void     *smap_itm, 
                                            rprt_f   *err_f     );              // identifier verification

void print_var_tok(const var_tok *var);                                         // var_tok printer
