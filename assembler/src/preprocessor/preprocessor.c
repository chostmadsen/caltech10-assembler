/**
 * src/preprocessor/preprocessor.c
 * Assembler preprocessor.
 */

#include    "output/errors.h"
#include    "output/messages.h"
#include    "common/kwrds.h"
#include    "common/hash_tables/pseudo.h"
#include    "reader/reader.h"

/*-PSEUDO-OP-VERIFIER-------------------------------------------------------------------------------------------------*/

/**
 * Verify all directives (pseudo-ops).
 *
 * @param       source          source file
 * @return                      whether there was an invalid pseudo-op
 */
[[nodiscard]] static bool pseudoop_chck_(const src_f *const source) {           // pseudo-op verifier
    cit10a_asrt(source != nullptr);

    // item setup
    bool    ret     =   false;
    rprt_f  err_f   =   { .file=source };

    // verify pseudo-ops
    for (size_t i = 0; i < source->ln_num; ++i) {
        // skip over non pseudo-op items
        strptr  sptr                =   { .str=src_f_getline(source, i), .ln=i, .col=0 };
        for (; is_whitespace(*sptr.str); inc_strptr(&sptr));
        if (*sptr.str != PSEUDO_STRT)   continue;

        // check pseudo-op
        inc_strptr(&sptr);
        size_t  n   =   0;
        for (; is_alphanum(sptr.str[n]); ++n);

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
                cit10a_msg( &(msg_info){ .type=msg_err_t, .header=".include", .report_f=&err_f },
                            ".include not yet supported"                                          );
                ret =   true;
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
    return  ret;
}

/*-PREPROCESSOR-------------------------------------------------------------------------------------------------------*/

/**
 * Preprocessor; currently doesn't do anything, just verifies all pseudo-ops.
 *
 * @param       source          source file
 * @return                      whether preprocessing was successful
 */
[[nodiscard]] bool preprocess(const src_f *const source) {                      // preprocessor
    return  pseudoop_chck_(source);
}
