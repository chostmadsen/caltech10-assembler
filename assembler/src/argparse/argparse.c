/**
 * src/argparse/argparse.c
 * Assembler argument parser.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>

#include    "helpers/general.h"
#include    "helpers/mem.h"
#include    "output/external.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "argparse/file_chck.h"
#include    "argparse/argparse.h"

/*-GLOBAL-COMPILE-ARGS-STRUCT-----------------------------------------------------------------------------------------*/

/**
 * Global compilation arguments; every stage references this, so global is preferred.
 */
assemble_args   c_args      =   { 0 };

/*-FLAG-USAGE-HELPER-FUNCTIONS----------------------------------------------------------------------------------------*/

/**
 * Flag usage printer.
 */
static void arg_help_msg_(void) {                                               // print assemble flag helper info
    // tab spacing (local consts)
    constexpr   int     tabs    =   24;
    constexpr   int     strt    =   7;

    // header
    cit10a_msg(&(msg_info){ .type=msg_norm_t, .header="compilation flags"}, nullptr);

    for (size_t i = 0; i < arr_s(a_flg); ++i) {
        // print flag info
        const   flag_itm    flg     =   *a_flg[i];
        printf("    " CLR_BOLD "-%c " CLR_DIM, flg.flag);
        switch (flg.type) {
            case flag_val_t:
                // find number print number
                const   int num_mn_prnt =   snprintf(nullptr, 0, "%d", flg.min);
                const   int num_mx_prnt =   snprintf(nullptr, 0, "%d", flg.max);
                cit10a_asrt(num_mn_prnt > 0 && num_mx_prnt > 0);
                const   int num_pad     =   num_mn_prnt + num_mx_prnt;
                printf("[ %d, %d ]\x1b[0m%*s", flg.min, flg.max, tabs - strt - num_pad - 6, "- ");
                break;
            case flag_str_t:    [[fallthrough]];
            case flag_strm_t:   [[fallthrough]];
            case flag_stra_t:
                printf("<%s>\x1b[0m%*s", flg.arg_itm, tabs - strt - 2 - (int)strlen(flg.arg_itm), "- ");
                break;
            case flag_bool_t:
                printf("\x1b[0m%*s", tabs - strt, "- ");
                break;
            default:
                cit10a_asrt(!"invalid flag type");
                cit10a_exit(INTRNL_ERRNO);
        }

        // flag description
        printf("%s", flg.desc);
        if (flg.exit)                   fputs(", then exit", stdout);
        if (flg.type == flag_val_t)     printf(" (default %d)", flg.dflt);
        fputc('\n', stdout);

        // further flag info
        if (flg.type == flag_str_t) {
            // flag argument usage
            for (size_t j = 0; j < flg.str_num; ++j) {
                printf( "%*s" CLR_BOLD CLR_DIM "%-*s\x1b[0m" CLR_DIM ": %s\x1b[0m\n",
                        strt, "", tabs - strt - 2, flg.strs[j].arg, flg.strs[j].desc  );
            }
        }
    }
}

/*-FLAG-PROCESSOR-HELPER-FUNCTIONS------------------------------------------------------------------------------------*/

/**
 * Get next boolean flag, if possible
 *
 * @param       flg             flag character
 * @param       error           error flag
 * @return                      next flag pointer if found, nullptr if not
 */
static const flag_itm *next_bool_f_(const char flg, bool *const error) {        // next boolean flag get
    const   msg_info    f_err   =   { .type=msg_err_t, .header="assembler flag error" };

    for (size_t j = 0; j < arr_s(a_flg); ++j) {
        const   flag_itm    *const  flag    =   a_flg[j];
        if (flg != flag->flag)                  continue;
        if (flag->type != flag_bool_t) {
            cit10a_msg(&f_err, "assembler flag `-%c` requires an argument", flag->flag);
            *error  =   true;
            return  nullptr;
        }
        return  flag;
    }
    cit10a_msg(&f_err, "unknown assembler flag `-%c`", flg);
    *error  =   true;
    return  nullptr;
}

/**
 * Proccesses boolean type flags. Modifies `c_args`.
 *
 * @param       arg             current argv
 * @param       flag            boolean flag
 * @param       error           error flag
 */
static void process_bool_f_( const char     *const  arg,
                             const flag_itm        *flag,
                                   bool     *const  error,
                                   bool     *const  end_comp ) {                // boolean flag processor
    int     ref_idx     =   1;
    for (;;) {
        if (flag != nullptr) {
            // set flag
            bool    *const  field   =   (bool*)((char*)&c_args + flag->offset);
            *field                  =   true;
            if (flag->exit)             *end_comp   =   true;
        }
        const   char    n_flg   =   arg[++ref_idx];
        if (n_flg == '\0')          return;
        flag                    =   next_bool_f_(n_flg, error);
    }
}

/**
 * Proccesses value type flags. Modifies `c_args`.
 *
 * @param       flag            value flag
 * @param       idx             current argv being processed
 * @param       argc            argument count
 * @param       argv            argument vector
 * @param       error           error flag
 */
static void process_val_f_( const flag_itm *const         flag,
                                  int      *const         idx, 
                            const int                     argc,
                            const char     *const *const  argv,
                                  bool     *const         error ) {             // value flag processor
    // set up references
    const   msg_info    f_err   =   { .type=msg_err_t, .header="assembler flag error" };
    const   char       *val;

    if (argv[*idx][2] != '\0') {
        // use attached value
        val     =   argv[*idx] + 2;
    } else {
        // use next value
        ++*idx;
        if (*idx >= argc || argv[*idx][0] == '-') {
            cit10a_msg(&f_err, "assembler flag `-%c` requires an integer-valued argument", flag->flag);
            *error  =   true;
            return;
        }
        val     =   argv[*idx];
    }

    // parse value
    char           *endptr;
    const   int     f_val   =   (int)strtol(val, &endptr, 10);
    if (*endptr != '\0') {
        cit10a_msg(&f_err, "assembler flag `-%c` requires an integer-value argument", flag->flag);
        *error  =   true;
        return;
    }

    // attempt to set field
    int     *const      field   =   (int*)((char*)&c_args + flag->offset);
    if (flag->min <= f_val && f_val <= flag->max) {
        *field      =   f_val;
        return;
    }
    cit10a_msg(&f_err, "assembler flag `-%c` has range [ %d, %d ]", flag->flag, flag->min, flag->max);
    *error  =   true;
}

/**
 * Proccesses string type flags. Modifies `c_args`.
 *
 * @param       flag            string flag
 * @param       idx             current argv being processed
 * @param       argc            argument count
 * @param       argv            argument vector
 * @param       error           error flag
 */
static void process_str_f_( const flag_itm *const         flag,
                                  int      *const         idx, 
                            const int                     argc,
                            const char     *const *const  argv,
                                  bool     *const         error ) {             // string flag processor
    // set up references
    const   msg_info    f_err   =   { .type=msg_err_t, .header="assembler flag error" };
    const   char       *str;

    if (argv[*idx][2] != '\0') {
        // use attached string
        str     =   argv[*idx] + 2;
    } else {
        // use next string
        ++*idx;
        if (*idx >= argc || argv[*idx][0] == '-') {
            cit10a_msg(&f_err, "assembler flag `-%c` requires a string argument", flag->flag);
            *error  =   true;
            return;
        }
        str     =   argv[*idx];
    }

    // parse field
    for (size_t i = 0; i < flag->str_num; ++i) {
        // check field item
        const   struct  str_f_t itm =   flag->strs[i];
        if (strcmp(str, itm.arg))       continue;

        // set field item
        char    *const  field       =   (char*)&c_args + flag->offset;
        bool    *const  field_itm   =   (bool*)(field + itm.offset);
        *field_itm                  =   true;
        return;
    }
    cit10a_msg(&f_err, "assembler flag `-%c` recieved an invalid argument", flag->flag);
    *error  =   true;
}

/**
 * Proccesses string type flags. Returns the actual char* of the argument.
 *
 * @param       flag            string (any type) flag
 * @param       idx             current argv being processed
 * @param       argc            argument count
 * @param       argv            argument vector
 * @param       error           error flag
 * @return                      char* argument
 */
[[nodiscard]] static const char *str_p_( const flag_itm *const         flag,
                                               int      *const         idx, 
                                         const int                     argc,
                                         const char     *const *const  argv,
                                               bool     *const         error ) {// string argument parser
    // set up references
    const   msg_info    f_err   =   { .type=msg_err_t, .header="assembler flag error" };
    const   char       *str;

    if (argv[*idx][2] != '\0') {
        // use attached string
        str     =   argv[*idx] + 2;
    } else {
        // use next string
        ++*idx;
        if (*idx >= argc || argv[*idx][0] == '-') {
            cit10a_msg(&f_err, "assembler flag `-%c` requires a string argument", flag->flag);
            *error  =   true;
            return  nullptr;
        }
        str     =   argv[*idx];
    }

    // set field
    return  str;
}

/**
 * Proccesses string (any type) type flags. Modifies `c_args`.
 *
 * @param       flag            string (any type) flag
 * @param       idx             current argv being processed
 * @param       argc            argument count
 * @param       argv            argument vector
 * @param       error           error flag
 */
static void process_stra_f_( const flag_itm *const         flag,
                                   int      *const         idx, 
                             const int                     argc,
                             const char     *const *const  argv,
                                   bool     *const         error ) {            // string (any type) flag processor
    // get flag
    const   char  **const   field   =   (const char**const)((char*)&c_args + flag->offset);
    *field                          =   str_p_(flag, idx, argc, argv, error);
}

/**
 * Proccesses string (multiple item) type flags. Modifies `c_args`.
 *
 * @param       flag            string (any type) flag
 * @param       idx             current argv being processed
 * @param       argc            argument count
 * @param       argv            argument vector
 * @param       error           error flag
 */
static void process_strm_f_( const flag_itm *const         flag,
                                   int      *const         idx, 
                             const int                     argc,
                             const char     *const *const  argv,
                                   bool     *const         error ) {            // string (multiple itms) flag processor
    // get return
    const   char    *const  strm_v  =   str_p_(flag, idx, argc, argv, error);
    if (strm_v == nullptr)              return;

    // add to array
    strm_arr        *const  field   =   (strm_arr*const)((char*)&c_args + flag->offset);

    if (field->size == 0) {
        field->size     =   1;
        field->vals     =   chckd_malloc(sizeof(char*) * field->size, "argparse field char**");
    } else if (field->num + 1 > field->size) {
        field->size     *=  2;
        field->vals     =   chckd_realloc(field->vals, sizeof(char*) * field->size, "argparse field char**");
    }
    field->vals[field->num++]   =   strm_v;
}

/*-ARGUMENT-PARSER----------------------------------------------------------------------------------------------------*/

/**
 * Resets assembler flags at `c_args`.
 */
void reset_args(void) {                                                         // reset assembler arguments
    memset(&c_args, 0, sizeof(c_args));

    // special field reset
    for (size_t i = 0; i < arr_s(a_flg); ++i) {
        const   flag_itm    *const  flag    =   a_flg[i];

        // set default value if applicable
        if (flag->type == flag_val_t) {
            int     *const  field   =   (int*)((char*)&c_args + flag->offset);
            *field                  =   flag->dflt;
        }
    }
}

/**
 * Compilation flag parser. Saves arguments to global variable `c_args`.
 * Use `reset_args` to reset `c_args`. Resetting should be done prior to every call.
 *
 * @param       argc            argument count
 * @param       argv            argument vector
 * @return                      whether to end assembly
 */
[[nodiscard]] bool parse_args( const int                argc,
                               const char *const *const argv  ) {                  // assembler argument parser
    cit10a_asrt(argv != nullptr);

    // reset args
    reset_args();

    // start assembler arguments
    bool    end_comp            =   false;

    // error message instances
    const   msg_info    f_err   =   { .type=msg_err_t, .header="assembler flag error" };
    const   msg_info    t_err   =   { .type=msg_err_t, .header="assembler target error" };

    // process flags
    bool                error   =   false;
    for (int i = 1; i < argc; ++i) {
        const   char    *const  arg     =   argv[i];

        // target item
        if (arg[0] != '-') {
            if (c_args.target == nullptr) {
                c_args.target   =   argv[i];
                // TODO : maybe check file extension? also maybe not? for now too lazy
            } else {
                cit10a_msg(&t_err, "multiple assembly targets given");
                error   =   true;
            }
            continue;
        }

        // verify flag item
        if (arg[1] == '\0') {
            cit10a_msg(&f_err, "no flag name after `-`");
            error   =   true;
            continue;
        }

        // flag item
        bool    found_flag  =   false;
        for (size_t j = 0; j < arr_s(a_flg); ++j) {
            const   flag_itm    *const  flag    =   a_flg[j];
            if (flag->flag != arg[1])               continue;
            switch (flag->type) {
                case flag_bool_t:
                    process_bool_f_(arg, flag, &error, &end_comp);
                    break;
                case flag_val_t:
                    process_val_f_(flag, &i, argc, argv, &error);
                    break;
                case flag_str_t:
                    process_str_f_(flag, &i, argc, argv, &error);
                    break;
                case flag_stra_t:
                    process_stra_f_(flag, &i, argc, argv, &error);
                    break;
                case flag_strm_t:
                    process_strm_f_(flag, &i, argc, argv, &error);
                    break;
                default:
                    cit10a_asrt(!"invalid flag type");
                    cit10a_exit(INTRNL_ERRNO);
            }
            found_flag  =   true;
            if (flag->exit)     end_comp    =   true;
            break;
        }

        // verify proper flag
        if (found_flag)         continue;
        cit10a_msg(&f_err, "unknown assembler flag `-%c`", arg[1]);
        error   =   true;
    }


    // process special flags
    if (c_args.help)            arg_help_msg_();
    if (c_args.version)         cit10a_info();


    // assembly target check
    if (c_args.target == nullptr && !end_comp) {
        // no build target
        cit10a_msg(&t_err, "no assembler targets given");
        error   =   true;
    } else if (!end_comp && !verify_target(c_args.target)) {
        // wrong file extension
        cit10a_msg(&t_err, "invalid source file extension");
        error   =   true;
    }

    // exit on error
    if (error) {
        cit10a_msg(&(msg_info){ .type=msg_norm_t, .header="use the -h flag for assembler flag usage" }, nullptr);
        cit10a_exit(ARGPARSE_ERRNO);
    }

    // get output
    if (c_args.target != nullptr)   c_args.output   =   get_output(c_args.target, c_args.output);

    // get processor number
    if (c_args.n_thrds == 0)        c_args.n_thrds  =   sysconf(_SC_NPROCESSORS_ONLN);
    return  end_comp;
}
