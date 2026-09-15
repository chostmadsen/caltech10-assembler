/**
 * src/preprocessor/preprocessor.c
 * Assembler preprocessor.
 */

#include    <stdio.h>
#include    <stddef.h>

#include    "helpers/general.h"
#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/stack.h"
#include    "common/kwrds.h"
#include    "common/hash_tables/pseudo.h"
#include    "common/gen_parse.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "preprocessor/folder_parse.h"
#include    "preprocessor/preprocessor.h"

/*-PSEUDO-OP-ITEMS----------------------------------------------------------------------------------------------------*/

static  stack           recursion_st;                                           // recursion stack

/*-PSEUDO-OP-HELPERS--------------------------------------------------------------------------------------------------*/

/**
 * Dumps the recursion stack for all the files that were opened.
 */
static void dump_recurs_st_(void) {                                             // dump the recursion stack
    cit10a_msg( &(msg_info){ .type=msg_norm_t, .header="lookthrough stack", .report_f=nullptr },
                "these are all the opened file(s), up until %d lookthrough(s)", c_args.max_recurs  );

    // print recursion stack
    const   int     prnt_pad    =   snprintf(nullptr, 0, "%d", c_args.max_recurs);
    for (int i = 0; i < recursion_st.len; ++i) {
        const   char    *const  f_name  =   *(char**)peek_stack(&recursion_st, i);
        printf(OUT_INDENT MSG_DELIM CLR_DIM "depth %*d : \x1b[0m%s\n", prnt_pad, i + 1, f_name);
    }
}

/**
 * Gets a source file from .include. nullptr if it could not be found.
 *
 * @param       sptr            string pointer
 * @param       srcs            sources
 * @param       err_f           error report file
 * @return                      found file, or nullptr
 */
[[nodiscard]] static src_f *get_incl_( strptr  *const sptr,
                                       sources *const srcs,
                                       rprt_f  *const err_f ) {                 // inclusion get
    // get string value
    for (; is_whitespace(*sptr->str); inc_strptr(sptr));
    err_f->col  =   sptr->col;
    bool                    has_err =   false;
    const   src_slice       f_name  =   parse_str(err_f, sptr, &has_err);
    if (has_err)                        return  nullptr;

    // check line end
    err_f->len  =   sptr->col - err_f->col + 1;
    inc_strptr(sptr);
    if (check_ln_end(sptr, err_f))      return  nullptr;

    // get source
    src_f   *const      ret     =   get_source(&f_name, srcs, err_f);
    if (ret == nullptr)             return nullptr;

    // add to recursion stack
    push_stack(&recursion_st, &ret->f_name);
    if (recursion_st.len >= c_args.max_recurs) {
        cit10a_msg( &(msg_info){ .type=msg_err_t, .header="maximum inclusion recursion reached", .report_f=err_f},
                    "maximum file inclusion recursion reached"                                                     );
        dump_recurs_st_();
        cit10a_exit(PREPROCESS_ERRNO);
    }
    return  ret;
}

/*-PSEUDO-OP-VERIFIER-------------------------------------------------------------------------------------------------*/

[[nodiscard]] static bool pseudoop_chck_(const src_f *source, sources *srcs);   // pseudo-op verifier

/**
 * Verify all .none sections (for constants, helps w/ output alignment).
 *
 * @param       source          source file
 * @param       srcs            sources
 * @param       end_chck        check for ending tail at current sptr position
 * @return                      whether there was an invalid none section
 */
[[nodiscard]] static bool verify_none_sctn_( rprt_f  *const err_f,
                                             sources *const srcs,
                                             strptr  *const sptr,
                                             bool           end_chck ) {        // .none verifier
    cit10a_asrt(err_f != nullptr);
    cit10a_asrt(sptr != nullptr);

    // check section end
    bool    ret     =   end_chck && check_ln_end(sptr, err_f);

    while (end_chck && !newln_strptr(sptr, err_f->file)) {
        end_chck    =   true;
        // skip whitespace
        for(; is_whitespace(*sptr->str); inc_strptr(sptr));

        // check item
        if (*sptr->str == CMMT_CHR || *sptr->str == '\0')       continue;

        // non-pseudo
        err_f->ln   =   sptr->ln;
        err_f->col  =   sptr->col;
        if (*sptr->str != PSEUDO_STRT) {
            // set up error file
            err_f->len  =   1;
            // error
            cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="ignored text", .report_f=err_f },
                        "non pseudo-op directives found in `.none`; prepend source with a directive." );
            continue;
        }

        // check pseudo-ops
        size_t  n   =   0;
        for (inc_strptr(sptr); is_alphanum(sptr->str[n]); ++n);
        err_f->len  =   n;

        // switch on pseudo-op
        switch (pseudo_hash_lu(sptr->str, n).tok) {
            case tok_data:  return  ret;
            case tok_code:  return  ret;
            case tok_org:
                cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="invalid .org", .report_f=err_f}, 
                            "ignored .org directive"                                                 );
                continue;
            case tok_none:  [[fallthrough]];
            case tok_const: break;
            case tok_incl:
                adj_strptr(sptr, n);
                src_f   *const  new_f   =   get_incl_(sptr, srcs, err_f);
                if (new_f == nullptr) {
                    ret =   true;
                    break;
                }
                if (pseudoop_chck_(new_f, srcs))    ret =   true;
                break;
            case pseudo_no_tok:
                cit10a_msg( &(msg_info){ .type=msg_err_t, .header="unknown directive", .report_f=err_f },
                            "unknown pseudo-op directive"                                                 );
                ret =   true;
                break;
            default:
                cit10a_asrt(!"invalid pseudo-op state");
                cit10a_exit(INTRNL_ERRNO);
        }
    }

    return  ret;
}

/**
 * Verify all directives (pseudo-ops).
 *
 * @param       source          source file
 * @param       srcs            sources
 * @return                      whether there was an invalid pseudo-op
 */
[[nodiscard]] static bool pseudoop_chck_( const src_f   *const source,
                                                sources *const srcs    ) {      // pseudo-op verifier
    cit10a_asrt(source != nullptr);

    // item setup
    bool    ret     =   false;
    rprt_f  err_f   =   { .file=source };
    strptr  sptr    =   { .str=src_f_getline(source, 0), .ln=0, .col=0 };
    if (verify_none_sctn_(&err_f, srcs, &sptr, false))      ret =   true;

    while (!newln_strptr(&sptr, source)) {
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));

        // skip over non-pseudo-op items
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check pseudo-op
        size_t  n   =   0;
        for (inc_strptr(&sptr); is_alphanum(sptr.str[n]); ++n);

        // set up error file
        err_f.ln    =   sptr.ln;
        err_f.col   =   sptr.col;
        err_f.len   =   n;

        // switch on pseudo-op
        switch (pseudo_hash_lu(sptr.str, n).tok) {
            case tok_data:  [[fallthrough]];
            case tok_code:  [[fallthrough]];
            case tok_org:   [[fallthrough]];
            case tok_const: break;
            case tok_incl:
                adj_strptr(&sptr, n);
                src_f   *const  new_f   =   get_incl_(&sptr, srcs, &err_f);
                if (new_f == nullptr) {
                    ret =   true;
                    break;
                }
                if (pseudoop_chck_(new_f, srcs))                    ret =   true;
                break;
            case tok_none:
                adj_strptr(&sptr, n);
                if (verify_none_sctn_(&err_f, srcs, &sptr, true))   ret =   true;
                break;
            case pseudo_no_tok:
                cit10a_msg( &(msg_info){ .type=msg_err_t, .header="unknown directive", .report_f=&err_f },
                            "unknown pseudo-op directive"                                                  );
                ret =   true;
                break;
            default:
                cit10a_asrt(!"invalid pseudo-op state");
                cit10a_exit(INTRNL_ERRNO);
        }
    }

    // return boolean flag
    rm_stack(&recursion_st);
    return  ret;
}

/*-PREPROCESSOR-------------------------------------------------------------------------------------------------------*/

/**
 * Preprocessor; currently doesn't do anything, just verifies all pseudo-ops.
 *
 * @param       source          source file
 * @param       srcs            sources
 */
void preprocess(const src_f *const source, sources *const srcs) {               // preprocessor
    recursion_st                        =   new_stack(sizeof(char**));
    push_stack(&recursion_st, &source->f_name);
    if (pseudoop_chck_(source, srcs))       cit10a_exit(PREPROCESS_ERRNO);
    free_stack(&recursion_st);
}
