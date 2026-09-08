/**
 * inc/common/kwrds.h
 * Keywords recognized by the assembler. IMPORTANT : Rehash on modification.
 */

#pragma     once

#include    "perfhash/inc/hash_table.h"

/*-GENERAL-ITEMS------------------------------------------------------------------------------------------------------*/

constexpr   char        CMMT_CHR            =   ';';                            // comment character
constexpr   char        NUM_SEP             =   '_';                            // number seperator
constexpr   char        NEG_SYMB            =   '-';                            // negative value symbol
constexpr   char        POS_SYMB            =   '+';                            // positive value symbol
constexpr   char        HEADER_CHR          =   ':';                            // header end character

/*-PSEUDO-OP-SPECIFIERS-----------------------------------------------------------------------------------------------*/
/**
 * NOTE : These aren't actually specified in any spec, I just made them up. If you want to change the actual string
 *        keywords for these, you can just change them here and the code will still work, after rehashing.
 *        (new identifiers must be alphanumeric)
 */

constexpr   char        PSEUDO_STRT         =   '.';                            // psuedo-op start

#define PSEUDO_SEGSPEC_T    /* str, tok, grp, _instr */                                                                \
    X( "data",      tok_data,   pseudo_segspec_t,  0 )                                                                 \
    X( "code",      tok_code,   pseudo_segspec_t,  0 )

#define PSEUDO_PSEUDOA_T    /* str, tok, grp, _instr */                                                                \
    X( "org",       tok_org,    pseudo_pseudoa_t,  0 )                                                                 \
    X( "include",   tok_incl,   pseudo_pseudoa_t,  0 )                                                                 \
    X( "const",     tok_const,  pseudo_pseudoa_t,  0 )

#define PSEUDO_T            /* macro, enum_t */                                                                        \
    Y( PSEUDO_SEGSPEC_T,    pseudo_segspec_t )                                                                         \
    Y( PSEUDO_PSEUDOA_T,    pseudo_pseudoa_t )

typedef enum {                                                                  // pseudo-op type enum
    pseudo_no_grp=0,
#define Y( _macro, enum_t )             enum_t,
    PSEUDO_T
#undef  Y
} pseudo_t;

typedef enum {                                                                  // pseudo-op token enum
    pseudo_no_tok=0,
#define Y( macro, _enum_t )             macro
#define X( _str, tok, _grp, _instr )    tok,
    PSEUDO_T
#undef  X
#undef  Y
} pseudo_tok;

typedef struct {                                                                // pseudo-op token struct
    const   pseudo_t        type;
    const   pseudo_tok      tok;
    const   int             instr;  /* unused */
} tok_pseudo;

static  const   hash_itm    pseudo_itms_[]  =   {                               // psueod-op hash group
#define Y( macro, _enum_t )             macro
#define X( strx, tokx, grpx, instrx )   { .str=strx,      .instr_str=#instrx,                                          \
                                          .tok_str=#tokx, .grp_str=#grpx,     .no_case=false },
    PSEUDO_T
#undef  X
#undef  Y
};

/*-STORAGE-TYPES------------------------------------------------------------------------------------------------------*/
/**
 * NOTE : I think this is derived from the 6502 or 8086 assembly, but I don't actually know how the machine instructions
 *        look for those (or if this CPU is even capable of those storage types) so I just limited data segment
 *        initialization a lot here. If anyone actually knows how dw works, or if initiazation is real, let me know.
 */

constexpr   char        STORAGE_IND         =   'd';                            // 'd' for data (6502 derived i think)
constexpr   char        STORAGE_BYTE        =   'b';                            // only byte data storage
constexpr   char        STORAGE_INIT        =   '?';                            // only storage init

/*-ALU-OPCODES--------------------------------------------------------------------------------------------------------*/

#define ALU_OFFSET_T        /* str, tok, grp, instr */                                                                 \
    X( "adc",       tok_adc,    alu_offset_t,       0x6000 )                                                           \
    X( "add",       tok_add,    alu_offset_t,       0x6800 )                                                           \
    X( "and",       tok_and,    alu_offset_t,       0x4400 )                                                           \
    X( "cmp",       tok_cmp,    alu_offset_t,       0x3000 )                                                           \
    X( "or",        tok_or,     alu_offset_t,       0x7400 )                                                           \
    X( "sbb",       tok_sbb,    alu_offset_t,       0x1800 )                                                           \
    X( "sub",       tok_sub,    alu_offset_t,       0x1000 )                                                           \
    X( "tst",       tok_tst,    alu_offset_t,       0x4c00 )                                                           \
    X( "xor",       tok_xor,    alu_offset_t,       0x3400 )

#define ALU_IMMEDIATE_T     /* str, tok, grp, instr */                                                                 \
    X( "adci",      tok_adci,   alu_immediate_t,    0x6300 )                                                           \
    X( "addi",      tok_addi,   alu_immediate_t,    0x6b00 )                                                           \
    X( "andi",      tok_andi,   alu_immediate_t,    0x4700 )                                                           \
    X( "cmpi",      tok_cmpi,   alu_immediate_t,    0x3300 )                                                           \
    X( "ori",       tok_ori,    alu_immediate_t,    0x7700 )                                                           \
    X( "sbbi",      tok_sbbi,   alu_immediate_t,    0x1b00 )                                                           \
    X( "subi",      tok_subi,   alu_immediate_t,    0x1300 )                                                           \
    X( "tsti",      tok_tsti,   alu_immediate_t,    0x4f00 )                                                           \
    X( "xori",      tok_xori,   alu_immediate_t,    0x3700 )

#define ALU_LONE_T          /* str, tok, grp, instr */                                                                 \
    X( "asr",       tok_asr,    alu_lone_t,         0x7101 )                                                           \
    X( "dec",       tok_dec,    alu_lone_t,         0x7b00 )                                                           \
    X( "inc",       tok_inc,    alu_lone_t,         0x0000 )                                                           \
    X( "lsl",       tok_lsl,    alu_lone_t,         0x5800 )                                                           \
    X( "lsr",       tok_lsr,    alu_lone_t,         0x7100 )                                                           \
    X( "neg",       tok_neg,    alu_lone_t,         0x2700 )                                                           \
    X( "not",       tok_not,    alu_lone_t,         0x2d00 )                                                           \
    X( "rlc",       tok_rlc,    alu_lone_t,         0x5000 )                                                           \
    X( "rol",       tok_rol,    alu_lone_t,         0x5200 )                                                           \
    X( "ror",       tok_ror,    alu_lone_t,         0x7102 )                                                           \
    X( "rrc",       tok_rrc,    alu_lone_t,         0x7103 )

#define ALU_T               /* macro, enum_t */                                                                        \
    Y( ALU_OFFSET_T,        alu_offset_t    )                                                                          \
    Y( ALU_IMMEDIATE_T,     alu_immediate_t )                                                                          \
    Y( ALU_LONE_T,          alu_lone_t      )

/*-FLAG-OPCODES-------------------------------------------------------------------------------------------------------*/

#define FLAG_T_T            /* str, tok, grp, instr */                                                                 \
    X( "sti",       tok_sti,    flag_t,             0x7f81 )                                                           \
    X( "cli",       tok_cli,    flag_t,             0x0769 )                                                           \
    X( "stu",       tok_stu,    flag_t,             0x7f22 )                                                           \
    X( "clu",       tok_clu,    flag_t,             0x07ca )                                                           \
    X( "stc",       tok_stc,    flag_t,             0x7f0c )                                                           \
    X( "clc",       tok_clc,    flag_t,             0x07e4 )

#define FLAG_T              /* macro, enum_t */                                                                        \
    Y( FLAG_T_T,            flag_t )

/*-INDEX-REGISTER-OPCODES---------------------------------------------------------------------------------------------*/

#define IND_REG_T_T         /* str, tok, grp, instr */                                                                 \
    X( "tax",       tok_tax,    ind_reg_t,          0x0780 )                                                           \
    X( "txa",       tok_txa,    ind_reg_t,          0x6701 )                                                           \
    X( "inx",       tok_inx,    ind_reg_t,          0x0580 )                                                           \
    X( "dex",       tok_dex,    ind_reg_t,          0x0d80 )                                                           \
    X( "tas",       tok_tas,    ind_reg_t,          0x0750 )                                                           \
    X( "tsa",       tok_tsa,    ind_reg_t,          0x6700 )                                                           \
    X( "ins",       tok_ins,    ind_reg_t,          0x0640 )                                                           \
    X( "des",       tok_des,    ind_reg_t,          0x0e40 )

#define IND_REG_T           /* macro, enum_t */                                                                        \
    Y( IND_REG_T_T,         ind_reg_t )

/*-LOAD-/-STORE-OPCODES-----------------------------------------------------------------------------------------------*/

#define LDST_IMMEDIATE_T    /* str, tok, grp, instr */                                                                 \
    X( "ldi",       tok_ldi,    ldst_immediate_t,   0x8900 )

#define LDST_DIRECT_T       /* str, tok, grp, instr */                                                                 \
    X( "ldd",       tok_ldd,    ldst_direct_t,      0x8000 )                                                           \
    X( "std",       tok_std,    ldst_direct_t,      0xa000 )

#define LDST_INDEXED_T      /* str, tok, grp, instr */                                                                 \
    X( "ld",        tok_ld,     ldst_indexed_t,     0x8000 )                                                           \
    X( "st",        tok_st,     ldst_indexed_t,     0xa000 )

#define LDST_T              /* macro, enum_t */                                                                        \
    Y( LDST_IMMEDIATE_T,    ldst_immediate_t )                                                                         \
    Y( LDST_DIRECT_T,       ldst_direct_t    )                                                                         \
    Y( LDST_INDEXED_T,      ldst_indexed_t   )

/*-BRANCH-OPCODES-----------------------------------------------------------------------------------------------------*/

#define JMP_ABSOLUTE_T      /* str, tok, grp, instr */                                                                 \
    X( "jmp",       tok_jmp,    jmp_absolute_t,     0xc000 )

#define JMP_RELATIVE_T      /* str, tok, grp, instr */                                                                 \
    X( "ja",        tok_ja,     jmp_relative_t,     0x8800 )                                                           \
    X( "jae",       tok_jae,    jmp_relative_t,     0x8c00 )                                                           \
    X( "jnc",       tok_jnc,    jmp_relative_t,     0x8c00 )  /* aka jae */                                            \
    X( "jb",        tok_jb,     jmp_relative_t,     0x8f00 )                                                           \
    X( "jc",        tok_jc,     jmp_relative_t,     0x8f00 )  /* aka jb */                                             \
    X( "jbe",       tok_jbe,    jmp_relative_t,     0x8b00 )                                                           \
    X( "je",        tok_je,     jmp_relative_t,     0x9f00 )                                                           \
    X( "jz",        tok_jz,     jmp_relative_t,     0x9f00 )                                                           \
    X( "jg",        tok_jg,     jmp_relative_t,     0xaf00 )                                                           \
    X( "jge",       tok_jge,    jmp_relative_t,     0xbb00 )                                                           \
    X( "jl",        tok_jl,     jmp_relative_t,     0xb800 )                                                           \
    X( "jle",       tok_jle,    jmp_relative_t,     0xac00 )                                                           \
    X( "jne",       tok_jne,    jmp_relative_t,     0x9c00 )                                                           \
    X( "jnz",       tok_jnz,    jmp_relative_t,     0x9c00 )  /* aka jne */                                            \
    X( "jns",       tok_jns,    jmp_relative_t,     0x9800 )                                                           \
    X( "jnu",       tok_jnu,    jmp_relative_t,     0xbc00 )                                                           \
    X( "jnv",       tok_jnv,    jmp_relative_t,     0xa800 )                                                           \
    X( "js",        tok_js,     jmp_relative_t,     0x9b00 )                                                           \
    X( "ju",        tok_ju,     jmp_relative_t,     0xbf00 )                                                           \
    X( "jv",        tok_jv,     jmp_relative_t,     0xab00 )

#define JMP_T               /* macro, enum_t */                                                                        \
    Y( JMP_ABSOLUTE_T,      jmp_absolute_t )                                                                           \
    Y( JMP_RELATIVE_T,      jmp_relative_t )

/*-SUBROUTINE-/-STACK-OPCODES-----------------------------------------------------------------------------------------*/

#define SUBROUT_ST_T_T      /* str, tok, grp, instr */                                                                 \
    X( "call",      tok_call,   subrout_st_t,       0xe000 )                                                           \
    X( "rts",       tok_rts,    subrout_st_t,       0x1f00 )                                                           \
    X( "popf",      tok_popf,   subrout_st_t,       0x0200 )                                                           \
    X( "pushf",     tok_pushf,  subrout_st_t,       0x0e00 )

#define SUBROUT_ST_T        /* macro, enum_t */                                                                        \
    Y( SUBROUT_ST_T_T,      subrout_st_t )

/*-IO-OPCODES---------------------------------------------------------------------------------------------------------*/

#define IO_T_T              /* str, tok, grp, instr */                                                                 \
    X( "in",        tok_in,     io_t,               0x9000 )                                                           \
    X( "out",       tok_out,    io_t,               0xb000 )

#define IO_T                /* macro, enum_t */                                                                        \
    Y( IO_T_T,              io_t )

/*-MISC-OPCODES-------------------------------------------------------------------------------------------------------*/

#define MISC_T_T            /* str, tok, grp, instr */                                                                 \
    X( "nop",       tok_nop,    misc_t,             0x1f80 )

#define MISC_T              /* macro, enum_t */                                                                        \
    Y( MISC_T_T,            misc_t )

/*-CONGLOMERATED-OPCODES----------------------------------------------------------------------------------------------*/
/**
 * NOTE : Derive addressing modes (if they apply) from the groups.
 *        A singular table provides O(1) lokup, so all opcodes are conglomerated here.
 */

#define OPCODE_T    ALU_T   FLAG_T  IND_REG_T   LDST_T  JMP_T   SUBROUT_ST_T    IO_T    MISC_T

typedef enum {                                                                  // opcode type enum
    opcode_no_grp=0,
#define Y( _macro, enum_t )             enum_t,
    OPCODE_T
#undef  Y
} opcode_t;

typedef enum {                                                                  // opcode token enum
    opcode_no_tok=0,
#define Y( macro, _enum_t )             macro
#define X( _str, tok, _grp, _instr )    tok,
    OPCODE_T
#undef  X
#undef  Y
} opcode_tok;

typedef struct {                                                                // opcode token struct
    const   opcode_t        type;
    const   opcode_tok      tok;
    const   int             instr;
} tok_opcode;

static  const   hash_itm    opcode_itms_[]  =   {                               // opcode hash group
#define Y( macro, _enum_t )             macro
#define X( strx, tokx, grpx, instrx )   { .str=strx,      .instr_str=#instrx,                                          \
                                          .tok_str=#tokx, .grp_str=#grpx,     .no_case=false },
    OPCODE_T
#undef  X
#undef  Y
};
