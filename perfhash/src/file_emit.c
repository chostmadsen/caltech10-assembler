/**
 * perfhash/src/file_emit.c
 * Perfect hashing file emission.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <stdint.h>
#include    <string.h>
#include    <inttypes.h>
#include    <locale.h>
#include    <stdarg.h>

#include    "hash_populate.h"
#include    "hash_table.h"
#include    "general_phs.h"
#include    "algorithm.h"
#include    "file_emit.h"

/*-FILE-EMISSION-HELPERS----------------------------------------------------------------------------------------------*/

/**
 * Return a full to-upper string.
 *
 * @param       str             string
 * @return                      upper-cased string
 */
[[nodiscard]] static char *to_upper_str_(const char *const str) {               // full string to upper
    char    *const  res     =   s_malloc(strlen(str) + 1);
    strcpy(res, str);
    char           *ptr     =   res;
    for (; *ptr != '\0'; ++ptr) {
        if ('a' <= *ptr && *ptr <= 'z')     *ptr    +=  'A' - 'a';
    }
    return  res;
}

/**
 * Find string padding for an integer.
 *
 * @param       num             number to find padding
 * @return                      padding
 */
[[nodiscard]] static int int_pad_(const int num) {                              // string integer pad
    int     ret     =   1;
    int     num_cmp =   9;
    while (num_cmp < num) {
        num_cmp =   1;
        for (int i = 0; i < ret + 1; ++i)   num_cmp *=  10;
        ++ret;
        num_cmp -=  1;
    }
    return  ret;
}

/**
 * Prints a formatted string w/ length returned.
 *
 * @param       fp              file pointer; nullptr for no write
 * @param       fmt             message format
 * @param       ...             variadic
 * @return                      number of characters printed
 */
static int fprintf_len_(FILE *const fp, const char *const fmt, ...) {           // formatted string printer w/ len
    // start variadic
    va_list         args;
    va_start(args, fmt);

    // allocate fmt print buf
    va_list         args_cpy;
    va_copy(args_cpy, args);
    const   int     msg_len =   vsnprintf(nullptr, 0, fmt, args_cpy);
    va_end(args_cpy);
    if (msg_len < 0) {
        fprintf(stderr, "vsnprintf failure\n");
        exit(1);
    }
    char    *const  msg_fmt =   s_malloc((size_t)msg_len + 1);

    // format
    vsnprintf(msg_fmt, (size_t)msg_len + 1, fmt, args);
    va_end(args);

    // print message
    if (fp != nullptr)          fwrite(msg_fmt, sizeof(char), msg_len, fp);
    free(msg_fmt);
    return  msg_len;
}

/**
 * Prints a formatted string with escape characters properly formatted w/ length returned.
 *
 * @param       fp              file pointer
 * @param       fmt             message format
 * @param       ...             variadic
 * @return                      number of characters printed
 */
static int fprintf_esc_(FILE *const fp, const char *const fmt, ...) {           // formatted string printer w/ len
    // start variadic
    va_list         args;
    va_start(args, fmt);

    // allocate fmt print buf
    va_list         args_cpy;
    va_copy(args_cpy, args);
    const   int     msg_len =   vsnprintf(nullptr, 0, fmt, args_cpy);
    va_end(args_cpy);
    if (msg_len < 0) {
        fprintf(stderr, "vsnprintf failure\n");
        exit(1);
    }
    char    *const  msg_fmt =   s_malloc((size_t)msg_len + 1);

    // format
    vsnprintf(msg_fmt, (size_t)msg_len + 1, fmt, args);
    va_end(args);

    // print message
    int             len     =   0;
    for (int i = 0; i < msg_len; ++i) {
        switch (msg_fmt[i]) {
            case '"':
                fputs("\\\"", fp);
                len +=  2;
                break;
            case '\\':
                fputs("\\\\", fp);
                len +=  2;
                break;
            default:
                fputc(msg_fmt[i], fp);
                ++len;
        }
    }
    free(msg_fmt);
    return  len;
}

/**
 * Prints a character a certain number of times.
 *
 * @param       fp              file pointer
 * @param       chr             character to print
 * @param       num             number of times to print character
 */
static void fprintf_repeat_(FILE *const fp, const char chr, const int num) {    // character repeat print
    for (int i = 0; i < num; ++i)       fputc(chr, fp);
}

/*-FULL-ALGORITHM-CALL------------------------------------------------------------------------------------------------*/

/**
 * Emits the full hash table to a file.
 *
 * @param       itm_arr         hash item array
 * @param       size            hash item array size
 * @param       seed            prng seed
 * @param       off_init        initial bucket offset
 * @param       max_attempts    maximum attempts
 * @param       inc             include file in generated file
 * @param       name            hash table name
 * @param       file            output file
 */
void hash_full( const hash_itm *const itm_arr,
                const size_t          size,
                const uint64_t        seed,
                const size_t          off_init,
                const size_t          max_attempts,
                const char     *const inc,
                const char     *const name,
                const char     *const file          ) {                         // perfect hashing full call
    // print setup
    setlocale(LC_NUMERIC, "");

    // hash algorithm
    hash_grp        group   =   new_hash_grp(itm_arr, size);
    printf("%s : ", name);
    const   hash_fn fn      =   algorithm(&group, seed, max_attempts, off_init);

    // find max string size
    int     str_max     =   0;
    int     tok_max     =   0;
    int     grp_max     =   0;
    int     instr_max   =   0;
    for (size_t i = 0; i < group.len; ++i) {
        if ((int)strlen(group.arr[i]->str) > (int)str_max)          str_max     =   (int)strlen(group.arr[i]->str);
        if ((int)strlen(group.arr[i]->tok_str) > (int)tok_max)      tok_max     =   (int)strlen(group.arr[i]->tok_str);
        if ((int)strlen(group.arr[i]->grp_str) > (int)grp_max)      grp_max     =   (int)strlen(group.arr[i]->grp_str);
        if ((int)strlen(group.arr[i]->instr_str) > (int)instr_max)  instr_max   =   (int)strlen(group.arr[i]->instr_str);
    }
    const   int     max_bck =   fprintf_len_(nullptr, "%zu", fn.buckets);

    // open file
    FILE    *const  fp  =   fopen(file, "w");
    if (fp == nullptr) {
        fprintf(stderr, "couldn't open file %s\n", file);
        exit(1);
    }

    // get name upper case
    char   *name_upper  =   to_upper_str_(name);

    // header
    fprintf(fp, "/**\n * %s\n", file);
    fprintf( fp,
             " * AUTO-GENERATED (seed %" PRIu64 ") - DO NOT MANUALLY MODIFY; "
             "see perfhash/README.md for usage.\n */\n\n", fn.seed             );
    fprintf(fp, "#ifndef %s_HASH_TABLE_\n", name_upper);
    fprintf(fp, "#define %s_HASH_TABLE_\n\n", name_upper);

    // inclusions
    fputs("#include    <stddef.h>\n", fp);
    fputs("#include    <string.h>\n", fp);
    fputs("#include    <stdint.h>\n\n", fp);
    fputs("#include    \"" HASH_TABLE_LOC "\"\n", fp);
    fprintf(fp, "#include    \"%s\"\n\n", inc);

    // constants
    fprintf_repeat_(fp, '-', LINE_MAX - 2 - fprintf_len_(fp, "/*-%s-CONSTANTS", name_upper));
    fprintf(fp, "*/\n\n");

    // table info (yes, i know this is a stupid way of doing this)
    constexpr   int     CEXPR_NAME_ALN  =   24;
    constexpr   int     CEXPR_EQ_ALN    =   44;
    enum info_state {
        offset_t=0, table_s_t,  max_str_t
    };
    for (int state = 0; state < max_str_t + 1; ++state) {
        // type
        int     ln_idx      =   0;
        const   char   *type;
        switch (state) {
            case offset_t:  type    =   "uint64_t"; break;
            case table_s_t: type    =   "size_t";   break;
            case max_str_t: type    =   "size_t";   break;
            default:
                fprintf(stderr, "invalid state");
                exit(1);
        }
        fprintf_repeat_(fp, ' ', CEXPR_NAME_ALN - fprintf_len_(fp, "constexpr   %s", type));
        ln_idx  =   CEXPR_NAME_ALN;

        // item
        const   char   *nm;
        switch (state) {
            case offset_t:  nm  =   "OFFSET";   break;
            case table_s_t: nm  =   "TBL_S";    break;
            case max_str_t: nm  =   "MAX_STR";  break;
            default:
                fprintf(stderr, "invalid state");
                exit(1);
        }
        fprintf_repeat_(fp, ' ', CEXPR_EQ_ALN - ln_idx - fprintf_len_(fp, "%s_%s", name_upper, nm));
        ln_idx  =   CEXPR_EQ_ALN;

        // value
        int             to_cmt_rem  =   CMT_ALN - ln_idx - fprintf_len_(fp, "=   ");
        const   char   *cmt;
        switch (state) {
            case offset_t:
                to_cmt_rem  -=  fprintf_len_(fp, "0x%016" PRIx64 ";", fn.offset);
                cmt         =   "hash offset";
                break;
            case table_s_t:
                to_cmt_rem  -=  fprintf_len_(fp, "%zu;", fn.buckets);
                cmt         =   "hash table size\n";
                break;
            case max_str_t:
                to_cmt_rem  -=  fprintf_len_(fp, "%d;", str_max);
                cmt         =   "hash table max string length";
                break;
            default:
                fprintf(stderr, "invalid state");
                exit(1);
        }

        // comment
        fprintf_repeat_(fp, ' ', to_cmt_rem);
        fprintf(fp, "// %s %s\n", name, cmt);
    }
    fputc('\n', fp);

    // table header
    fprintf_repeat_(fp, '-', LINE_MAX - 2 - fprintf_len_(fp, "/*-%s-HASH-TABLE", name_upper));
    fprintf(fp, "*/\n\n");

    // table definition print
    int     num_prnt    =   fprintf_len_(fp, "static  const   hash_entry  %s_table[%s_TBL_S] ", name, name_upper);
    while (num_prnt++ % TABSTOP) {
        fputc(' ', fp);
    }
    num_prnt            +=  fprintf_len_(fp, "=   {");
    fprintf_repeat_(fp, ' ', CMT_ALN + 1 - num_prnt);
    fprintf(fp, "// %s hash table\n", name);

    // table info print
    constexpr   int     TABLE_EQ_ALN    =   12;
    for (size_t i = 0; i < group.len; ++i) {
        // hash item info
        const   hash_itm    *const  itm     =   group.arr[i];
        const   uint64_t            hash    =   hash_fn_impl(itm->str, fn.offset);
        const   size_t              bucket  =   (size_t)(hash & (fn.buckets - 1));
        // bucket
        fprintf_repeat_(fp, ' ', TABLE_EQ_ALN - fprintf_len_(fp, "    [%*zu]", max_bck, bucket));

        // hash
        fprintf_len_(fp, "=   { 0x%016" PRIx64 ", \"", hash);

        // string length
        const   int emitted =   fprintf_esc_(fp, "%s", group.arr[i]->str);
        fprintf(fp, "\", %*zu, ", str_max + int_pad_(str_max) - emitted, strlen(itm->str));

        // token
        fprintf(fp, "{ .tok=");
        fprintf_repeat_(fp, ' ', tok_max + 1 - fprintf_len_(fp, "%s,", itm->tok_str));

        // group
        fprintf(fp, " .grp=");
        fprintf_repeat_(fp, ' ', grp_max + 1 - fprintf_len_(fp, "%s,", itm->grp_str));

        // instruction
        fprintf(fp, " .instr=");
        fprintf_repeat_(fp, ' ', instr_max - fprintf_len_(fp, "%s", itm->instr_str));

        // end
        fprintf(fp, " } }%s\n", (i != group.len - 1) ? "," : "");
    }
    fputs("};\n\n", fp);

    // hash function
    fprintf_repeat_(fp, '-', LINE_MAX - 2 - fprintf_len_(fp, "/*-%s-HASH-FUNCTION", name_upper));
    fprintf(fp, "*/\n\n");

    fprintf(fp, "/**\n"
                " * Generates the hash for the %s hash table based on a string and number of characters, "
                "lowercased.\n"
                " *\n"
                " * @param       str             string to hash\n"
                " * @param       n               character number\n"
                " * @return                      hash\n"
                " */\n", 
                name                                                                                       );

    fprintf_repeat_( fp, ' ',
                     CMT_ALN -
                     fprintf_len_(fp, "[[nodiscard]] static uint64_t %s_hash_fn(const char *str, size_t n) {", name) );
    fprintf(fp, "// %s hash function\n", name);

    fprintf(fp, "    uint64_t    hash    =   %s_OFFSET;\n"
                "    while (n--) {\n"
                "        const   char    chr =   *(str++);\n"
                "        hash                ^=  chr | (uint8_t)((chr - 'A') < 26) << 5;\n"
                "        hash                *=  FNV_PRIME_64_HT;\n"
                "    }\n"
                "    return  hash ^ (hash >> 32);\n"
                "}\n\n",
                name_upper                                                                  );
    
    // hash table lookup functions
    fprintf_repeat_(fp, '-', LINE_MAX - 2 - fprintf_len_(fp, "/*-%s-HASH-TABLE-LOOKUP-FUNCTION", name_upper));
    fprintf(fp, "*/\n\n");

    // hash lookup function
    fprintf(fp, "/**\n"
                " * Performs a lookup in the %s hash table based on the provided string and hash "
                "(case insensitive).\n"
                " * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)\n"
                " *\n"
                " * @param       str             lookup string\n"
                " * @param       n               character number\n"
                " * @param       hash            string hash\n"
                " * @return                      table entry\n"
                " */\n",
                name                                                                               );

    const   int to_paren    =   fprintf_len_(fp, "[[nodiscard]] static tok_itm %s_hash_lu_h(", name);
    fprintf(fp, " const char     *const str,\n");
    fprintf_repeat_(fp, ' ', to_paren);
    fprintf(fp, " const size_t          n,\n");
    int         to_cmt      =   CMT_ALN - to_paren;
    fprintf_repeat_(fp, ' ', to_paren);
    fprintf_repeat_(fp, ' ', to_cmt - fprintf_len_(fp, " const uint64_t        hash ) {"));
    fprintf(fp, "// %s hash table lookup (w/ hash)\n", name);

    constexpr   int ret_pad =   20;
    fprintf(fp, "    if (n > %s_TBL_S) %*sreturn (tok_itm){ .tok=0, .grp=0 };\n\n"
                "    // get table entry\n"
                "    const   hash_entry  table_entry =   %s_table[hash & (%s_TBL_S - 1)];\n\n"
                "    // check entry\n"
                "    if (n != table_entry.len)                           return  (tok_itm){ .tok=0, .grp=0 };\n"
                "    if (hash != table_entry.hash)                       return  (tok_itm){ .tok=0, .grp=0 };\n"
                "    for(unsigned i = 0; i < table_entry.len; ++i) {\n"
                "        const   char    chr_t   =   "
                "table_entry.str[i] | (uint8_t)((table_entry.str[i] - 'A') < 26) << 5;\n"
                "        const   char    chr_l   =   str[i] | (uint8_t)((str[i] - 'A') < 26) << 5;\n"
                "        if (chr_t != chr_l)         return  (tok_itm){ .tok=0, .grp=0 };\n"
                "    }\n\n"
                "    // return matched item\n"
                "    return  table_entry.itm;\n"
                "}\n\n",
                name_upper, ret_pad - (int)strlen(name_upper), "", name, name_upper                              );

    // string lookup function
    fprintf(fp, "/**\n"
                " * Performs a lookup in the %s hash table based on the provided string (case insensitive).\n"
                " * (tok_itm){ .tok=0, .grp=0 } on lookup failure. (.instr uninitialized)\n"
                " *\n"
                " * @param       str             lookup string\n"
                " * @param       n               character number\n"
                " * @return                      table entry\n"
                " */\n",
                name                                                                                           );
    fprintf(fp, "[[maybe_unused]] [[nodiscard]]\n");
    fprintf_repeat_( fp, ' ',
                     CMT_ALN -
                     fprintf_len_(fp, "static tok_itm %s_hash_lu(const char *const str, const size_t n) {",
                                      name                                                                  ) );
    fprintf(fp, "// %s hash table lookup\n", name);

    fprintf(fp, "    // generate hash and lookup\n"
                "    return  %s_hash_lu_h(str, n, %s_hash_fn(str, n));\n"
                "}\n\n",
                name, name                                                );

    // end
    fprintf(fp, "#endif  /* %s_HASH_TABLE_ */\n", name_upper);

    // free items
    free_hash_grp(&group);
    free(name_upper);
}
