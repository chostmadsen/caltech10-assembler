/**
 * inc/common/hash_tables/opcode.h
 * AUTO-GENERATED - DO NOT MANUALLY MODIFY; see perfhash/README.md for usage.
 */

#ifndef OPCODE_HASH_TABLE_
#define OPCODE_HASH_TABLE_

#include    <stddef.h>
#include    <string.h>
#include    <stdint.h>

#include    "perfhash/inc/hash_table.h"
#include    "common/kwrds.h"

/*-OPCODE-CONSTANTS---------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    OPCODE_OFFSET       =   0x370237a98ffd4d5d;             // opcode hash offset
constexpr   size_t      OPCODE_TBL_S        =   256;                            // opcode hash table size

constexpr   size_t      OPCODE_MAX_STR      =   5;                              // opcode hash table max string length

/*-OPCODE-HASH-TABLE--------------------------------------------------------------------------------------------------*/

static  const   hash_entry  opcode_table[OPCODE_TBL_S]  =   {                   // opcode hash table
    [111]   =   { 0x615a6ce6418df26f, "adc",   3, { .tok=tok_adc,   .grp=alu_offset_t,     .instr=0x6000 } },
    [218]   =   { 0x615a6de6418dcdda, "add",   3, { .tok=tok_add,   .grp=alu_offset_t,     .instr=0x6800 } },
    [216]   =   { 0x617c61e6418813d8, "and",   3, { .tok=tok_and,   .grp=alu_offset_t,     .instr=0x4400 } },
    [153]   =   { 0x7389b5e658ebfa99, "cmp",   3, { .tok=tok_cmp,   .grp=alu_offset_t,     .instr=0x3000 } },
    [156]   =   { 0x0a60ded04f072f9c, "or",    2, { .tok=tok_or,    .grp=alu_offset_t,     .instr=0x7400 } },
    [107]   =   { 0xe9cf3de53451a06b, "sbb",   3, { .tok=tok_sbb,   .grp=alu_offset_t,     .instr=0x1800 } },
    [152]   =   { 0xea1d27e537fdbe98, "sub",   3, { .tok=tok_sub,   .grp=alu_offset_t,     .instr=0x1000 } },
    [ 81]   =   { 0xbeb362e57bf18951, "tst",   3, { .tok=tok_tst,   .grp=alu_offset_t,     .instr=0x4c00 } },
    [ 99]   =   { 0x9ad4ace52a4db863, "xor",   3, { .tok=tok_xor,   .grp=alu_offset_t,     .instr=0x3400 } },
    [169]   =   { 0x4445eb098a271da9, "adci",  4, { .tok=tok_adci,  .grp=alu_immediate_t,  .instr=0x6300 } },
    [102]   =   { 0x444913098a2c6366, "addi",  4, { .tok=tok_addi,  .grp=alu_immediate_t,  .instr=0x6b00 } },
    [220]   =   { 0x9accb1096592f8dc, "andi",  4, { .tok=tok_andi,  .grp=alu_immediate_t,  .instr=0x4700 } },
    [121]   =   { 0xb54f2c1b0d434e79, "cmpi",  4, { .tok=tok_cmpi,  .grp=alu_immediate_t,  .instr=0x3300 } },
    [ 58]   =   { 0x0a8bc0e5e515013a, "ori",   3, { .tok=tok_ori,   .grp=alu_immediate_t,  .instr=0x7700 } },
    [ 18]   =   { 0xe9c414977d425b12, "sbbi",  4, { .tok=tok_sbbi,  .grp=alu_immediate_t,  .instr=0x1b00 } },
    [100]   =   { 0xb023df98b487c364, "subi",  4, { .tok=tok_subi,  .grp=alu_immediate_t,  .instr=0x1300 } },
    [233]   =   { 0x4dbce96e7d0a21e9, "tsti",  4, { .tok=tok_tsti,  .grp=alu_immediate_t,  .instr=0x4f00 } },
    [ 86]   =   { 0xb076b94ba4682b56, "xori",  4, { .tok=tok_xori,  .grp=alu_immediate_t,  .instr=0x3700 } },
    [243]   =   { 0x613b65e6418787f3, "asr",   3, { .tok=tok_asr,   .grp=alu_lone_t,       .instr=0x7101 } },
    [189]   =   { 0x484f01e65aa32ebd, "dec",   3, { .tok=tok_dec,   .grp=alu_lone_t,       .instr=0x7b00 } },
    [202]   =   { 0x1cd32ce5e6ed6cca, "inc",   3, { .tok=tok_inc,   .grp=alu_lone_t,       .instr=0x0000 } },
    [209]   =   { 0x0325b2e5e8efbbd1, "lsl",   3, { .tok=tok_lsl,   .grp=alu_lone_t,       .instr=0x5800 } },
    [227]   =   { 0x032598e5e8ec45e3, "lsr",   3, { .tok=tok_lsr,   .grp=alu_lone_t,       .instr=0x7100 } },
    [188]   =   { 0x152fdfe5e31ab1bc, "neg",   3, { .tok=tok_neg,   .grp=alu_lone_t,       .instr=0x2700 } },
    [215]   =   { 0x151af0e5e339dfd7, "not",   3, { .tok=tok_not,   .grp=alu_lone_t,       .instr=0x2d00 } },
    [  5]   =   { 0xf17d37e510cdde05, "rlc",   3, { .tok=tok_rlc,   .grp=alu_lone_t,       .instr=0x5000 } },
    [203]   =   { 0xf18064e5103308cb, "rol",   3, { .tok=tok_rol,   .grp=alu_lone_t,       .instr=0x5200 } },
    [ 41]   =   { 0xf1804ee510330829, "ror",   3, { .tok=tok_ror,   .grp=alu_lone_t,       .instr=0x7102 } },
    [191]   =   { 0xf1766fe510dd45bf, "rrc",   3, { .tok=tok_rrc,   .grp=alu_lone_t,       .instr=0x7103 } },
    [108]   =   { 0xea1a38e537c4b86c, "sti",   3, { .tok=tok_sti,   .grp=flag_t,           .instr=0x7f81 } },
    [119]   =   { 0x7386a8e658e6ab77, "cli",   3, { .tok=tok_cli,   .grp=flag_t,           .instr=0x0769 } },
    [ 88]   =   { 0xea1a34e537c44d58, "stu",   3, { .tok=tok_stu,   .grp=flag_t,           .instr=0x7f22 } },
    [ 19]   =   { 0x7386b4e658e6a313, "clu",   3, { .tok=tok_clu,   .grp=flag_t,           .instr=0x07ca } },
    [ 94]   =   { 0xea1a3ee537c4b45e, "stc",   3, { .tok=tok_stc,   .grp=flag_t,           .instr=0x7f0c } },
    [117]   =   { 0x73869ee658d96c75, "clc",   3, { .tok=tok_clc,   .grp=flag_t,           .instr=0x07e4 } },
    [ 23]   =   { 0xbe8362e57b9aa517, "tax",   3, { .tok=tok_tax,   .grp=ind_reg_t,        .instr=0x0780 } },
    [221]   =   { 0xbea24fe57b9617dd, "txa",   3, { .tok=tok_txa,   .grp=ind_reg_t,        .instr=0x6701 } },
    [175]   =   { 0x1cd325e5e6ed11af, "inx",   3, { .tok=tok_inx,   .grp=ind_reg_t,        .instr=0x0580 } },
    [160]   =   { 0x484eeae65aa2e2a0, "dex",   3, { .tok=tok_dex,   .grp=ind_reg_t,        .instr=0x0d80 } },
    [ 50]   =   { 0xbe8369e57b9aba32, "tas",   3, { .tok=tok_tas,   .grp=ind_reg_t,        .instr=0x0750 } },
    [142]   =   { 0xbeb34fe57bf1848e, "tsa",   3, { .tok=tok_tsa,   .grp=ind_reg_t,        .instr=0x6700 } },
    [ 26]   =   { 0x1cd31ce5e6ed381a, "ins",   3, { .tok=tok_ins,   .grp=ind_reg_t,        .instr=0x0640 } },
    [205]   =   { 0x484ef1e65aa2e5cd, "des",   3, { .tok=tok_des,   .grp=ind_reg_t,        .instr=0x0e40 } },
    [ 33]   =   { 0x030697e5e8a9bd21, "ldi",   3, { .tok=tok_ldi,   .grp=ldst_immediate_t, .instr=0x8900 } },
    [246]   =   { 0x03068ce5e8a994f6, "ldd",   3, { .tok=tok_ldd,   .grp=ldst_direct_t,    .instr=0x8000 } },
    [223]   =   { 0xea1a43e537c4d0df, "std",   3, { .tok=tok_std,   .grp=ldst_direct_t,    .instr=0xa000 } },
    [149]   =   { 0x0a5dced04f386e95, "ld",    2, { .tok=tok_ld,    .grp=ldst_indexed_t,   .instr=0x8000 } },
    [106]   =   { 0x0a53ecd04f0e926a, "st",    2, { .tok=tok_st,    .grp=ldst_indexed_t,   .instr=0xa000 } },
    [ 78]   =   { 0x36229ce63e41184e, "jmp",   3, { .tok=tok_jmp,   .grp=jmp_absolute_t,   .instr=0xc000 } },
    [ 44]   =   { 0x0a71edd04f07902c, "ja",    2, { .tok=tok_ja,    .grp=jmp_relative_t,   .instr=0x8800 } },
    [ 29]   =   { 0x3614b1e63e43db1d, "jae",   3, { .tok=tok_jae,   .grp=jmp_relative_t,   .instr=0x8c00 } },
    [  0]   =   { 0x362c7fe63e47d800, "jnc",   3, { .tok=tok_jnc,   .grp=jmp_relative_t,   .instr=0x8c00 } },
    [197]   =   { 0x0a71f0d04f0773c5, "jb",    2, { .tok=tok_jb,    .grp=jmp_relative_t,   .instr=0x8f00 } },
    [178]   =   { 0x0a71efd04f076eb2, "jc",    2, { .tok=tok_jc,    .grp=jmp_relative_t,   .instr=0x8f00 } },
    [182]   =   { 0x361fa1e63e7ef6b6, "jbe",   3, { .tok=tok_jbe,   .grp=jmp_relative_t,   .instr=0x8b00 } },
    [224]   =   { 0x0a71e9d04f079ee0, "je",    2, { .tok=tok_je,    .grp=jmp_relative_t,   .instr=0x9f00 } },
    [173]   =   { 0x0a71e8d04f079dad, "jz",    2, { .tok=tok_jz,    .grp=jmp_relative_t,   .instr=0x9f00 } },
    [ 70]   =   { 0x0a71ebd04f079146, "jg",    2, { .tok=tok_jg,    .grp=jmp_relative_t,   .instr=0xaf00 } },
    [ 15]   =   { 0x360ea5e63e5c4f0f, "jge",   3, { .tok=tok_jge,   .grp=jmp_relative_t,   .instr=0xbb00 } },
    [171]   =   { 0x0a71f2d04f0774ab, "jl",    2, { .tok=tok_jl,    .grp=jmp_relative_t,   .instr=0xb800 } },
    [ 28]   =   { 0x3625b5e63e40511c, "jle",   3, { .tok=tok_jle,   .grp=jmp_relative_t,   .instr=0xac00 } },
    [170]   =   { 0x362c81e63e472aaa, "jne",   3, { .tok=tok_jne,   .grp=jmp_relative_t,   .instr=0x9c00 } },
    [135]   =   { 0x362c98e63e474a87, "jnz",   3, { .tok=tok_jnz,   .grp=jmp_relative_t,   .instr=0x9c00 } },
    [240]   =   { 0x362c8fe63e474cf0, "jns",   3, { .tok=tok_jns,   .grp=jmp_relative_t,   .instr=0x9800 } },
    [154]   =   { 0x362c91e63e47579a, "jnu",   3, { .tok=tok_jnu,   .grp=jmp_relative_t,   .instr=0xbc00 } },
    [115]   =   { 0x362c94e63e475f73, "jnv",   3, { .tok=tok_jnv,   .grp=jmp_relative_t,   .instr=0xa800 } },
    [226]   =   { 0x0a71dfd04f07b9e2, "js",    2, { .tok=tok_js,    .grp=jmp_relative_t,   .instr=0x9b00 } },
    [208]   =   { 0x0a71d9d04f0785d0, "ju",    2, { .tok=tok_ju,    .grp=jmp_relative_t,   .instr=0xbf00 } },
    [201]   =   { 0x0a71dcd04f07bdc9, "jv",    2, { .tok=tok_jv,    .grp=jmp_relative_t,   .instr=0xab00 } },
    [250]   =   { 0xd92dcd1b159bd8fa, "call",  4, { .tok=tok_call,  .grp=subrout_st_t,     .instr=0xe000 } },
    [109]   =   { 0xf16257e510f80f6d, "rts",   3, { .tok=tok_rts,   .grp=subrout_st_t,     .instr=0x1f00 } },
    [134]   =   { 0x0f37ba8cd606f586, "popf",  4, { .tok=tok_popf,  .grp=subrout_st_t,     .instr=0x0200 } },
    [ 69]   =   { 0x1a3e8d9c7d25bb45, "pushf", 5, { .tok=tok_pushf, .grp=subrout_st_t,     .instr=0x0e00 } },
    [ 38]   =   { 0x0a67f2d04f09c326, "in",    2, { .tok=tok_in,    .grp=io_t,             .instr=0x9000 } },
    [ 82]   =   { 0x0a88d5e5e5147a52, "out",   3, { .tok=tok_out,   .grp=io_t,             .instr=0xb000 } },
    [131]   =   { 0x151aece5e339c483, "nop",   3, { .tok=tok_nop,   .grp=misc_t,           .instr=0x1f80 } }
};

/*-OPCODE-HASH-FUNCTION-----------------------------------------------------------------------------------------------*/

/**
 * Generates the hash for the opcode hash table based on a string and number of characters, lowercased.
 *
 * @param       str             string to hash
 * @param       n               character number
 * @return                      hash
 */
[[nodiscard]] static uint64_t opcode_hash_fn(const char *str, size_t n) {       // opcode hash function
    uint64_t    hash    =   OPCODE_OFFSET;
    while (n--) {
        const   char    chr =   *(str++);
        hash                ^=  chr | (uint8_t)((chr - 'A') < 26) << 5;
        hash                *=  FNV_PRIME_64_HT;
    }
    return  hash ^ (hash >> 32);
}

/*-OPCODE-HASH-TABLE-LOOKUP-FUNCTION----------------------------------------------------------------------------------*/

/**
 * Performs a lookup in the opcode hash table based on the provided string and hash (case insensitive).
 * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)
 *
 * @param       str             lookup string
 * @param       n               character number
 * @param       hash            string hash
 * @return                      table entry
 */
[[nodiscard]] static tok_itm opcode_hash_lu_h( const char     *const str,
                                               const size_t          n,
                                               const uint64_t        hash ) {   // opcode hash table lookup (w/ hash)
    // get table entry
    const   hash_entry  table_entry =   opcode_table[hash & (OPCODE_TBL_S - 1)];

    // check entry
    if (n != table_entry.len)                           return  (tok_itm){ .tok=0, .grp=0 };
    if (hash != table_entry.hash)                       return  (tok_itm){ .tok=0, .grp=0 };
    for(unsigned i = 0; i < table_entry.len; ++i) {
        const   char    chr_t   =   table_entry.str[i] | (uint8_t)((table_entry.str[i] - 'A') < 26) << 5;
        const   char    chr_l   =   str[i] | (uint8_t)((str[i] - 'A') < 26) << 5;
        if (chr_t != chr_l)         return  (tok_itm){ .tok=0, .grp=0 };
    }

    // return matched item
    return  table_entry.itm;
}

/**
 * Performs a lookup in the opcode hash table based on the provided string (case insensitive).
 * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)
 *
 * @param       str             lookup string
 * @param       n               character number
 * @return                      table entry
 */
[[maybe_unused]] [[nodiscard]]
static tok_itm opcode_hash_lu(const char *const str, const size_t n) {          // opcode hash table lookup
    // generate hash and lookup
    return  opcode_hash_lu_h(str, n, opcode_hash_fn(str, n));
}

#endif  /* OPCODE_HASH_TABLE_ */
