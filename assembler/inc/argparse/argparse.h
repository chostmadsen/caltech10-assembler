/**
 * inc/argparse/argparse.h
 * documentation @ src/argparse/argparse
 */

#pragma     once

#include    <stddef.h>

#include    "helpers/general.h"

/*-FLAG-BASE-INFORMATION-ITEMS----------------------------------------------------------------------------------------*/

struct str_f_t {                                                                // string flag argument item
    const   char       *const   arg;
    const   size_t              offset;
    const   char       *const   desc;
};

typedef struct {                                                                // multiple string array
    const   char              **vals;
            int                 num;
            int                 size;
} strm_arr;

typedef enum {                                                                  // flag types
    flag_bool_t,    flag_val_t,     flag_str_t,     flag_stra_t,    flag_strm_t
} flag_type;
typedef struct {                                                                // flag info struct
    const   char                flag;
    const   flag_type           type;
    const   size_t              offset;
    const   bool                exit;
    union {
        struct {                                                                // value flag arguments
            const   int         min;
            const   int         max;
            const   int         dflt;
        };
        struct {                                                                // string flag arguments
            const           size_t              str_num;
            const   struct  str_f_t     *const  strs;
        };
    };
    const   char       *const   arg_itm;
    const   char       *const   desc;
} flag_itm;

/*-FLAG-SPECIFIC-INFORMATION-ITEMS------------------------------------------------------------------------------------*/

struct wrn_t_ {                                                                 // warning options
    bool                off;
    bool                err;
    bool                noverc;
    bool                noverd;
};

struct emit_t_ {                                                                // emit options
    bool                file;
    bool                table;
    bool                aasm;
};

typedef struct {                                                                // assemble flags struct
    // flags
            int         verbosity;
    struct  wrn_t_      warnings;
    struct  emit_t_     emit;
            int         max_errs;
            int         n_thrds;
            bool        case_sens;
            bool        bin;

    // exit on set flags
            bool        help;
            bool        version;

    // assembler sources
    const   char       *target;
    strm_arr            inc;
    strm_arr            src;

    // assembler target
    const   char       *output;
} assemble_args;

/*-FLAG-ITEMS---------------------------------------------------------------------------------------------------------*/

static  const   flag_itm        verbosity_f =   { .flag='v',            .desc="verbosity of assembly",
                                                  .type=flag_val_t,     .offset=offsetof(assemble_args, verbosity),
                                                  .exit=false,          .arg_itm="verbosity",
                                                  .min=0,               .max=4,
                                                  .dflt=0                                                             };

static  const   struct  str_f_t wrn_itm_[]  =   { { .arg="off",         .desc="warnings off",
                                                    .offset=offsetof(struct wrn_t_, off)                            },
                                                  { .arg="error",       .desc="warnings as errors",
                                                    .offset=offsetof(struct wrn_t_, err)                            },
                                                  { .arg="novercode",   .desc="suppress overlap warnings in .code",
                                                    .offset=offsetof(struct wrn_t_, noverc)                         },
                                                  { .arg="noverdata",   .desc="suppress overlap warnings in .data",
                                                    .offset=offsetof(struct wrn_t_, noverd)                         } };
static  const   flag_itm        warnings_f  =   { .flag='W',            .desc="assembly warning message level",
                                                  .type=flag_str_t,     .offset=offsetof(assemble_args, warnings), 
                                                  .exit=false,          .arg_itm="level",
                                                  .strs=wrn_itm_,       .str_num=arr_s(wrn_itm_)                      };

static  const   struct  str_f_t emit_itm_[] =   { { .arg="file",        .desc="emit read file",
                                                    .offset=offsetof(struct emit_t_, file)                          },
                                                  { .arg="table",       .desc="emit generated lookup tables",
                                                    .offset=offsetof(struct emit_t_, table)                         },
                                                  { .arg="asm",         .desc="emit asm",
                                                    .offset=offsetof(struct emit_t_, aasm)                          } };
static  const   flag_itm        emit_f      =   { .flag='E',            .desc="assembly stage emitter toggle",
                                                  .type=flag_str_t,     .offset=offsetof(assemble_args, emit),
                                                  .exit=false,          .arg_itm="stage",
                                                  .strs=emit_itm_,      .str_num=arr_s(emit_itm_)                     };

static  const   flag_itm        max_errs_f  =   { .flag='e',            .desc="maximum allowed errors before exit",
                                                  .type=flag_val_t,     .offset=offsetof(assemble_args, max_errs),
                                                  .exit=false,          .arg_itm="number",
                                                  .min=1,               .max=0x7fff,
                                                  .dflt=20                                                            };

static  const   flag_itm        bin_f       =   { .flag='b',            .desc="emit binary only",
                                                  .type=flag_bool_t,    .offset=offsetof(assemble_args, bin),
                                                  .exit=false,          .arg_itm=nullptr                              };

static  const   flag_itm        case_sens_f =   { .flag='C',            .desc="case-sensitive identifiers",
                                                  .type=flag_bool_t,    .offset=offsetof(assemble_args, case_sens),
                                                  .exit=false,          .arg_itm=nullptr                              };

static  const   flag_itm        n_thrds_f   =   { .flag='t',            .desc="maximum number of threads deployed "
                                                                              "(0 to use processor count)",
                                                  .type=flag_val_t,     .offset=offsetof(assemble_args, n_thrds),
                                                  .exit=false,          .arg_itm="number",
                                                  .min=0,               .max=0x7fff,
                                                  .dflt=0                                                             };

static  const   flag_itm        help_f      =   { .flag='h',            .desc="print assembler flag usage information",
                                                  .type=flag_bool_t,    .offset=offsetof(assemble_args, help),
                                                  .exit=true,           .arg_itm=nullptr                              };

static  const   flag_itm        version_f   =   { .flag='V',            .desc="print assembler version and build type",
                                                  .type=flag_bool_t,    .offset=offsetof(assemble_args, version),
                                                  .exit=true,           .arg_itm=nullptr                              };

static  const   flag_itm        inc_f       =   { .flag='I',            .desc="assembly inclusions directory",
                                                  .type=flag_strm_t,    .offset=offsetof(assemble_args, inc),
                                                  .exit=false,          .arg_itm="include"                            };

static  const   flag_itm        sourc_f     =   { .flag='S',            .desc="assembly secondary sources directory",
                                                  .type=flag_strm_t,    .offset=offsetof(assemble_args, src),
                                                  .exit=false,          .arg_itm="source"                             };


static  const   flag_itm        output_f    =   { .flag='o',            .desc="assembly output target",
                                                  .type=flag_stra_t,    .offset=offsetof(assemble_args, output),
                                                  .exit=false,          .arg_itm="output"                             };

// flag item arrays
static  const   flag_itm    *const  a_flg[] =   { &verbosity_f, &warnings_f,    &emit_f,    &max_errs_f,    &bin_f,
                                                  &case_sens_f, &n_thrds_f,     &help_f,    &version_f,     
                                                  &inc_f,       &sourc_f,       &output_f                           };

/*-GLOBAL-ASSEMBLE-ARGS-STRUCT----------------------------------------------------------------------------------------*/

extern  assemble_args    c_args;                                                // assemble flag struct item

/*-FUNCTIONS----------------------------------------------------------------------------------------------------------*/

void reset_args(void);                                                          // reset assembler arguments
[[nodiscard]] bool parse_args(int arc, const char *const *argv);                // assembler argument parser
