/**
 * inc/common/hash_tables/opcode.h
 * AUTO-GENERATED (seed 1) - DO NOT MANUALLY MODIFY; see perfhash/README.md for usage.
 */

#ifndef OPCODE_HASH_TABLE_
#define OPCODE_HASH_TABLE_

#include    <stddef.h>
#include    <string.h>
#include    <stdint.h>

#include    "perfhash/inc/hash_table.h"
#include    "common/kwrds.h"

/*-OPCODE-CONSTANTS---------------------------------------------------------------------------------------------------*/

constexpr   uint64_t    OPCODE_OFFSET       =   0xcae10843e6caf943;             // opcode hash offset
constexpr   size_t      OPCODE_TBL_S        =   256;                            // opcode hash table size

constexpr   size_t      OPCODE_MAX_STR      =   5;                              // opcode hash table max string length

/*-OPCODE-HASH-TABLE--------------------------------------------------------------------------------------------------*/

static  const   hash_entry  opcode_table[OPCODE_TBL_S]  =   {                   // opcode hash table
    [188]   =   { 0x5c0f2e63be06d5bc, "adc",   3, { .tok=tok_adc,   .grp=alu_offset_t,      .instr=0x6000 } },
    [165]   =   { 0x5c0f2b63be06dda5, "add",   3, { .tok=tok_add,   .grp=alu_offset_t,      .instr=0x6800 } },
    [247]   =   { 0x5c235763be39bdf7, "and",   3, { .tok=tok_and,   .grp=alu_offset_t,      .instr=0x4400 } },
    [102]   =   { 0x4a1cef639dafe566, "cmp",   3, { .tok=tok_cmp,   .grp=alu_offset_t,      .instr=0x3000 } },
    [ 89]   =   { 0xe3b1d01b007a0459, "or",    2, { .tok=tok_or,    .grp=alu_offset_t,      .instr=0x7400 } },
    [216]   =   { 0xd71b8b64ff26b0d8, "sbb",   3, { .tok=tok_sbb,   .grp=alu_offset_t,      .instr=0x1800 } },
    [  3]   =   { 0xd6ef9d64fef78203, "sub",   3, { .tok=tok_sub,   .grp=alu_offset_t,      .instr=0x1000 } },
    [150]   =   { 0x10f7986458ce3496, "tst",   3, { .tok=tok_tst,   .grp=alu_offset_t,      .instr=0x4c00 } },
    [236]   =   { 0x34d64664683530ec, "xor",   3, { .tok=tok_xor,   .grp=alu_offset_t,      .instr=0x3400 } },
    [251]   =   { 0x77c789b961313ffb, "adci",  4, { .tok=tok_adci,  .grp=alu_immediate_t,   .instr=0x6300 } },
    [228]   =   { 0x77bd69b9615342e4, "addi",  4, { .tok=tok_addi,  .grp=alu_immediate_t,   .instr=0x6b00 } },
    [ 94]   =   { 0xaaf87bb99945305e, "andi",  4, { .tok=tok_andi,  .grp=alu_immediate_t,   .instr=0x4700 } },
    [ 35]   =   { 0xa23532a7270f8723, "cmpi",  4, { .tok=tok_cmpi,  .grp=alu_immediate_t,   .instr=0x3300 } },
    [117]   =   { 0xb2f8ca64a1a14f75, "ori",   3, { .tok=tok_ori,   .grp=alu_immediate_t,   .instr=0x7700 } },
    [223]   =   { 0xc109b030a1051adf, "sbbi",  4, { .tok=tok_sbbi,  .grp=alu_immediate_t,   .instr=0x1b00 } },
    [250]   =   { 0x51477f3071bbbbfa, "subi",  4, { .tok=tok_subi,  .grp=alu_immediate_t,   .instr=0x1300 } },
    [  7]   =   { 0x0e648d66b464c607, "tsti",  4, { .tok=tok_tsti,  .grp=alu_immediate_t,   .instr=0x4f00 } },
    [218]   =   { 0xab907d897d137dda, "xori",  4, { .tok=tok_xori,  .grp=alu_immediate_t,   .instr=0x3700 } },
    [132]   =   { 0x5c504f63be118584, "asr",   3, { .tok=tok_asr,   .grp=alu_lone_t,        .instr=0x7101 } },
    [122]   =   { 0x875af3637dd45f7a, "dec",   3, { .tok=tok_dec,   .grp=alu_lone_t,        .instr=0x7b00 } },
    [141]   =   { 0xa1398e64a8147f8d, "inc",   3, { .tok=tok_inc,   .grp=alu_lone_t,        .instr=0x0000 } },
    [ 22]   =   { 0xcc854864ed37c716, "lsl",   3, { .tok=tok_lsl,   .grp=alu_lone_t,        .instr=0x5800 } },
    [108]   =   { 0xcc855a64ed37f46c, "lsr",   3, { .tok=tok_lsr,   .grp=alu_lone_t,        .instr=0x7100 } },
    [231]   =   { 0xba7b0d64ad3c1fe7, "neg",   3, { .tok=tok_neg,   .grp=alu_lone_t,        .instr=0x2700 } },
    [ 16]   =   { 0xba8f0a64add8bc10, "not",   3, { .tok=tok_not,   .grp=alu_lone_t,        .instr=0x2d00 } },
    [190]   =   { 0xde33c564f5e3d8be, "rlc",   3, { .tok=tok_rlc,   .grp=alu_lone_t,        .instr=0x5000 } },
    [ 28]   =   { 0xde299664f5eeef1c, "rol",   3, { .tok=tok_rol,   .grp=alu_lone_t,        .instr=0x5200 } },
    [ 38]   =   { 0xde29a464f5ee3526, "ror",   3, { .tok=tok_ror,   .grp=alu_lone_t,        .instr=0x7102 } },
    [  4]   =   { 0xde3a8d64f5ef5004, "rrc",   3, { .tok=tok_rrc,   .grp=alu_lone_t,        .instr=0x7103 } },
    [ 27]   =   { 0xd6eb7264fef89f1b, "sti",   3, { .tok=tok_sti,   .grp=flag_t,            .instr=0x7f81 } },
    [116]   =   { 0x4a18e2639db7e974, "cli",   3, { .tok=tok_cli,   .grp=flag_t,            .instr=0x0769 } },
    [ 31]   =   { 0xd6eb8664feff891f, "stu",   3, { .tok=tok_stu,   .grp=flag_t,            .instr=0x7f22 } },
    [128]   =   { 0x4a18e6639db7f780, "clu",   3, { .tok=tok_clu,   .grp=flag_t,            .instr=0x07ca } },
    [ 25]   =   { 0xd6eb7c64fef88219, "stc",   3, { .tok=tok_stc,   .grp=flag_t,            .instr=0x7f0c } },
    [134]   =   { 0x4a18dc639db7dc86, "clc",   3, { .tok=tok_clc,   .grp=flag_t,            .instr=0x07e4 } },
    [ 40]   =   { 0x1126a06459478b28, "tax",   3, { .tok=tok_tax,   .grp=ind_reg_t,         .instr=0x0780 } },
    [230]   =   { 0x110fad645941cfe6, "txa",   3, { .tok=tok_txa,   .grp=ind_reg_t,         .instr=0x6701 } },
    [ 92]   =   { 0xa1398364a8145c5c, "inx",   3, { .tok=tok_inx,   .grp=ind_reg_t,         .instr=0x0580 } },
    [171]   =   { 0x875b08637dd5c7ab, "dex",   3, { .tok=tok_dex,   .grp=ind_reg_t,         .instr=0x0d80 } },
    [153]   =   { 0x1126ab6459479699, "tas",   3, { .tok=tok_tas,   .grp=ind_reg_t,         .instr=0x0750 } },
    [109]   =   { 0x10f7a56458ce666d, "tsa",   3, { .tok=tok_tsa,   .grp=ind_reg_t,         .instr=0x6700 } },
    [221]   =   { 0xa1397e64a814a8dd, "ins",   3, { .tok=tok_ins,   .grp=ind_reg_t,         .instr=0x0640 } },
    [ 42]   =   { 0x875b03637dd5c42a, "des",   3, { .tok=tok_des,   .grp=ind_reg_t,         .instr=0x0e40 } },
    [146]   =   { 0xccaa6564ed7bb992, "ldi",   3, { .tok=tok_ldi,   .grp=ldst_immediate_t,  .instr=0x8900 } },
    [105]   =   { 0xccaa7264ed7b8169, "ldd",   3, { .tok=tok_ldd,   .grp=ldst_direct_t,     .instr=0x8000 } },
    [252]   =   { 0xd6eb7564fef887fc, "std",   3, { .tok=tok_std,   .grp=ldst_direct_t,     .instr=0xa000 } },
    [ 64]   =   { 0xe3bbec1b006fb440, "ld",    2, { .tok=tok_ld,    .grp=ldst_indexed_t,    .instr=0x8000 } },
    [119]   =   { 0xe3bfca1b0067cd77, "st",    2, { .tok=tok_st,    .grp=ldst_indexed_t,    .instr=0xa000 } },
    [  2]   =   { 0x998756649c900502, "jmp",   3, { .tok=tok_jmp,   .grp=jmp_absolute_t,    .instr=0xc000 } },
    [ 37]   =   { 0xe3a7cb1b0064bc25, "ja",    2, { .tok=tok_ja,    .grp=jmp_relative_t,    .instr=0x8800 } },
    [197]   =   { 0x99957b649cb6b4c5, "jae",   3, { .tok=tok_jae,   .grp=jmp_relative_t,    .instr=0x8c00 } },
    [176]   =   { 0x99847d649c9122b0, "jnc",   3, { .tok=tok_jnc,   .grp=jmp_relative_t,    .instr=0x8c00 } },
    [144]   =   { 0xe3a7ca1b0064bf90, "jb",    2, { .tok=tok_jb,    .grp=jmp_relative_t,    .instr=0x8f00 } },
    [195]   =   { 0xe3a7c91b0064bac3, "jc",    2, { .tok=tok_jc,    .grp=jmp_relative_t,    .instr=0x8f00 } },
    [ 14]   =   { 0x99925b649cb3380e, "jbe",   3, { .tok=tok_jbe,   .grp=jmp_relative_t,    .instr=0x8b00 } },
    [ 17]   =   { 0xe3a7cf1b0064b111, "je",    2, { .tok=tok_je,    .grp=jmp_relative_t,    .instr=0x9f00 } },
    [ 56]   =   { 0xe3a7d21b00645138, "jz",    2, { .tok=tok_jz,    .grp=jmp_relative_t,    .instr=0x9f00 } },
    [191]   =   { 0xe3a7cd1b0064b7bf, "jg",    2, { .tok=tok_jg,    .grp=jmp_relative_t,    .instr=0xaf00 } },
    [151]   =   { 0x999c47649cb5d297, "jge",   3, { .tok=tok_jge,   .grp=jmp_relative_t,    .instr=0xbb00 } },
    [ 62]   =   { 0xe3a7c81b0064ba3e, "jl",    2, { .tok=tok_jl,    .grp=jmp_relative_t,    .instr=0xb800 } },
    [164]   =   { 0x998b47649c9065a4, "jle",   3, { .tok=tok_jle,   .grp=jmp_relative_t,    .instr=0xac00 } },
    [ 10]   =   { 0x99847b649c91270a, "jne",   3, { .tok=tok_jne,   .grp=jmp_relative_t,    .instr=0x9c00 } },
    [219]   =   { 0x998466649c915edb, "jnz",   3, { .tok=tok_jnz,   .grp=jmp_relative_t,    .instr=0x9c00 } },
    [192]   =   { 0x99846d649c9129c0, "jns",   3, { .tok=tok_jns,   .grp=jmp_relative_t,    .instr=0x9800 } },
    [ 90]   =   { 0x99846b649c912a5a, "jnu",   3, { .tok=tok_jnu,   .grp=jmp_relative_t,    .instr=0xbc00 } },
    [239]   =   { 0x99846a649c9155ef, "jnv",   3, { .tok=tok_jnv,   .grp=jmp_relative_t,    .instr=0xa800 } },
    [ 19]   =   { 0xe3a7d91b00645613, "js",    2, { .tok=tok_js,    .grp=jmp_relative_t,    .instr=0x9b00 } },
    [ 33]   =   { 0xe3a7df1b00644621, "ju",    2, { .tok=tok_ju,    .grp=jmp_relative_t,    .instr=0xbf00 } },
    [156]   =   { 0xe3a7de1b0064499c, "jv",    2, { .tok=tok_jv,    .grp=jmp_relative_t,    .instr=0xab00 } },
    [148]   =   { 0x39c843a770392e94, "call",  4, { .tok=tok_call,  .grp=subrout_st_adrs_t, .instr=0xe000 } },
    [ 86]   =   { 0xde4ea564f5a80a56, "rts",   3, { .tok=tok_rts,   .grp=subrout_st_lone_t, .instr=0x1f00 } },
    [ 72]   =   { 0x49687a4847e31e48, "popf",  4, { .tok=tok_popf,  .grp=subrout_st_lone_t, .instr=0x0200 } },
    [ 74]   =   { 0x4cda702925cb204a, "pushf", 5, { .tok=tok_pushf, .grp=subrout_st_lone_t, .instr=0x0e00 } },
    [139]   =   { 0xe3aae41b006f338b, "in",    2, { .tok=tok_in,    .grp=io_t,              .instr=0x9000 } },
    [145]   =   { 0xb2e7d764a1acf091, "out",   3, { .tok=tok_out,   .grp=io_t,              .instr=0xb000 } },
    [204]   =   { 0xba8f0664add8a9cc, "nop",   3, { .tok=tok_nop,   .grp=misc_t,            .instr=0x1f80 } }
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
    if (n > OPCODE_TBL_S)               return (tok_itm){ .tok=0, .grp=0 };

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
