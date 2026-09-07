/**
 * perfhash/src/hash_populate.c
 * Perfect hashing hashtable population implementation.
 */

#include    <stddef.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

#include    "general_phs.h"
#include    "hash_table.h"
#include    "hash_populate.h"

/*-POPULATION-HELPER-FUNCTIONS----------------------------------------------------------------------------------------*/

/**
 * Hash item duplicate (keeps allocation location consistent).
 * 
 * @param       itm             hash item
 */
[[nodiscard]] hash_itm *cp_hash_itm_(const hash_itm *const itm) {               // hash item duplicator
    // copy item
    hash_itm    *const  itm_c   =   s_malloc(sizeof(hash_itm));
    memcpy(itm_c, itm, sizeof(hash_itm));

    // copy string
    char        *const  str_c   =   s_malloc((strlen(itm->str) + 1) * sizeof(char));
    strcpy(str_c, itm->str);
    itm_c->str                  =   str_c;

    return  itm_c;
}

/**
 * Hash group item push w/ realloc check.
 *
 * @param       grp             hash group
 * @param       itm             item to push
 */
static void push_itm_(hash_grp *const grp, const hash_itm *const itm) {         // hash group push
    if (grp->len >= grp->size) {
        grp->size           *=  2;
        grp->arr            =   s_realloc(grp->arr, grp->size * sizeof(hash_itm*));
    }
    grp->arr[grp->len++]    =   itm;
}

/**
 * Recursive case variant emission.
 *
 * @param       itm             hash item
 * @param       str_idx         current string index
 * @param       grp             hash group
 */
static void emit_case_variants_( const hash_itm *const itm,
                                       size_t          str_idx, 
                                       hash_grp *const grp      ) {             // case variant emitter
    for (char chr; (chr = itm->str[str_idx]) != '\0'; ++str_idx) {
        // find adjustment
        signed  char    adj;
        if      ('A' <= chr && chr <= 'Z')      adj =   'a' - 'A';
        else if ('a' <= chr && chr <= 'z')      adj =   'A' - 'a';
        else                                    continue;

        // modify string
        char        *const  str_c   =   s_malloc((strlen(itm->str) + 1) * sizeof(char));
        strcpy(str_c, itm->str);
        str_c[str_idx]              +=  adj;

        // copy itm and assign string
        hash_itm    *const  itm_c   =   cp_hash_itm_(itm);
        free((void*)itm_c->str);
        itm_c->str                  =   str_c;
        emit_case_variants_(itm_c, str_idx + 1, grp);
    }
    push_itm_(grp, itm);
}

/**
 * Duplicate item hash check.
 *
 * @param       grp             hash group
 */
static void dup_chck_(const hash_grp *const grp) {                              // duplicate check
    for (size_t i = 0; i < grp->len; ++i) {
        for (size_t j = 0; j < grp->len; ++j) {
            if (j == i)                                         continue;
            if (strcmp(grp->arr[i]->str, grp->arr[j]->str))     continue;
            fprintf(stderr, "duplicate key for %s, %s\n", grp->arr[i]->tok_str, grp->arr[j]->tok_str);
            exit(1);
        }
    }
}

/*-HASH-GROUP-POPULATION-FREEING-AND-PRINTING-------------------------------------------------------------------------*/

/**
 * Hash group setup and population.
 *
 * @param       itm_arr         hash item array
 * @param       size            hash item array size
 * @return                      populated hash group
 */
[[nodiscard]] hash_grp new_hash_grp( const hash_itm *const itm_arr,
                                     const size_t          size     ) {         // hash group setup
    // initialize string group
    hash_grp    group   =   { .arr=s_malloc(sizeof(hash_itm*)), .len=0, .size=1 };

    // populate string group
    for (size_t i = 0; i < size; ++i) {
        const   hash_itm    *const  itm_c   =   cp_hash_itm_(itm_arr + i);

        // case dependent
        if (!itm_arr[i].no_case) {
            push_itm_(&group, itm_c);
            continue;
        }

        // case independent
        emit_case_variants_(itm_c, 0, &group);
    }
    dup_chck_(&group);
    return  group;
}

/**
 * Hash group printing.
 *
 * @param       group           hash group
 */
[[maybe_unused]] void print_hash_grp(const hash_grp *const group) {             // hash group printer
    for (size_t i = 0; i < group->len; ++i) {
        const   hash_itm    itm     =   *group->arr[i];
        printf("\"%s\" : %s::%s\n", itm.str, itm.tok_str, itm.grp_str);
    }
}

/**
 * Hash group freeing.
 *
 * @param       group       hash group
 */
void free_hash_grp(hash_grp *const group) {                                     // hash group free
    for (size_t i = 0; i < group->len; ++i)     free((void*)group->arr[i]->str);
    free(group->arr);
}
