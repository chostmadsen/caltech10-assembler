/**
 * perfhash/impl/assembler_main.c
 * Main tokenizer hashing call.
 */

#include    <stddef.h>

#include    "general_phs.h"
#include    "file_emit.h"
#include    "assembler/inc/common/kwrds.h"

/**
 * Main tokenizer hashing call.
 *
 * @return                      exit code
 */
#include    <stdio.h>
int main(void) {                                                                // main
    // algorithm setup
    const   size_t              max     =   100'000'000;

    // output setup
    const   char        *const  inc     =   "common/kwrds.h";

    // hash
    hash_full(pseudo_itms_, arr_s(pseudo_itms_), 0, 0, max, inc, "pseudo", "inc/common/hash_tables/pseudo.h");
    hash_full(opcode_itms_, arr_s(opcode_itms_), 0, 0, max, inc, "opcode", "inc/common/hash_tables/opcode.h");

    // program end
    return  0;
}
