/**
 * src/preprocessor/folder_parse.
 * Checks the files in the provided folder path, and reads them into the proper structs.
 */

#include    <stdarg.h>
#include    <dirent.h>
#include    <string.h>
#include    <limits.h>

#include    "common/gen_parse.h"
#include    "helpers/general.h"
#include    "helpers/mem.h"
#include    "output/errors.h"
#include    "output/messages.h"
#include    "datastructures/hash.h"
#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "argparse/argparse.h"
#include    "argparse/file_chck.h"
#include    "reader/reader.h"
#include    "preprocessor/folder_parse.h"

/*-FILE-GATHERER-HELPERS----------------------------------------------------------------------------------------------*/

/**
 * Finds the stackmap size based on the maximum number of elements `n` for file stackmap.
 *
 * @param       n               maximum number of elements
 * @return                      stackmap size
 */
[[nodiscard]] static size_t fmap_s_(size_t n) {                                 // flatmap bucket get
    // evil bit hack to catch overflow
    cit10a_asrt(n != 0);
    cit10a_asrt((((size_t)-1 >> (FMAP_SHFT + 1)) + 1) >= n);

    // shift over and return
    if (n == 1)     return  1 << FMAP_SHFT;
    n           =   (n - 1) << FMAP_SHFT;
    for (unsigned i = 1; i < sizeof(size_t) * CHAR_BIT; i <<= 1)    n   |=  n >> i;
    return  n + 1;
}

/**
 * Check if the file should be ignored (. and ..).
 *
 * @param       f_name          file name
 * @return                      whether the file should be skipped
 */
[[nodiscard]] static bool ignore_f_(const char *const f_name) {                 // file skip check
    cit10a_asrt(f_name != nullptr);
    return  (f_name[0] == '.' && (f_name[1] == '\0' || (f_name[1] == '.' && f_name[2] == '\0')));
}

/**
 * Opens a directory, then checks how many files that follow the proper file extension typing are within that directory.
 * This number is incremented in a passed size_t*. Then, returns the opened directory (ensure to close this).
 *
 * @param       flag            flag that flagged this directory
 * @param       dir             directory name
 * @param       total           total number of files (to be incremented)
 * @param       f_name          file name
 * @return                      open directory
 */
[[nodiscard]] DIR *dir_scan_( const char          flag,
                              const char   *const dir,
                                    size_t *const total ) {                     // initial directory scanner
    cit10a_asrt(dir != nullptr);
    cit10a_asrt(total != nullptr);

    DIR *const  dp  =   opendir(dir);
    if (dp == nullptr) {
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="directory open failure", .report_f=nullptr },
                    "could not open directory `%s` (from -%c%s)", dir, flag, dir                          );
        return  nullptr;
    }

    // get extensions - update if this changes
    const   char    (*extns)[ECL];
    size_t          extns_n;
    if (flag == INC_FLG) {
        extns   =   INC_EXTNS;
        extns_n =   arr_s(INC_EXTNS);
    } else if (flag == SRC_FLG) {
        extns   =   SRC_EXTNS;
        extns_n =   arr_s(SRC_EXTNS);
    } else {
        cit10a_asrt(!"invalid directory scan flag");
        cit10a_exit(INTRNL_ERRNO);
    }

    struct  dirent *ent;
    while ((ent = readdir(dp)) != nullptr) {
        // skip . and ..
        if (ignore_f_(ent->d_name))         continue;

        // count sources
        for (size_t i = 0; i < extns_n; ++i) {
            if (strcmp(extns[i], file_extension(ent->d_name)))      continue;
            ++(*total);
            break;
        }
    }
    return  dp;
}

/**
 * Gets the filename with escaped characters, for lookup within the file.
 *
 * @param       fname           file name
 * @return                      file name in code recognition
 */
[[nodiscard]] static src_slice fname_esc_(const char *const fname) {            // filename with escaped characters
    // find malloc size
    const   size_t  f_len   =   strlen(fname);
    size_t          m_size  =   1;
    const   rprt_f  err_f   =   { .file=&(src_f){ .f_name=(char*)fname }, .len=0 };
    for (size_t i = 0; i < f_len; ++i) {
        if (fname[i] == ESC_CHR || fname[i] == '"')     m_size  +=  ESC_CHR_LEN;
        ++m_size;
    }

    // filename init
    size_t          idx         =   0;
    char    *const  fname_esc   =   chckd_malloc(m_size, "adjusted file name char*");
    fname_esc[m_size - 1]       =   '\0';
    for (size_t i = 0; i < f_len; ++i) {
        // setup recognized filename
        if (fname[i] == ESC_CHR || fname[i] == '"')     fname_esc[idx++]    =   ESC_CHR;
        fname_esc[idx++]    =   fname[i];
        if (' ' <= fname[i] && fname[i] <= '~')         continue;
        cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="filename", .report_f=&err_f}, 
                    "the assembler has no way of identifying the character `%c` for this filename in code "
                    "(please just be normal with your filenames)", fname[i]                                 );
    }
    return  (src_slice){ .str=fname_esc, .len=m_size - 1 };
}

/*-FILE-GATHERER-SINGULAR-SETUP---------------------------------------------------------------------------------------*/

/**
 * Sets up the source group for a singular flag item. This source struct will either be just the flag, with the .empty
 * field flagged as true, or the flag, stackmaps for the file names, and stackmaps for the cached source files, with the
 * .empty field flagged as false.
 *
 * Cached files will be opened and read into a buffer, similar to all other files. Ensure that this is properly cleaned.
 *
 * @param       strm            multiple string item
 * @return                      source group for flag
 */
[[nodiscard]] static src_grp file_setup_(strm_arr *const strm) {                // singular file setup
    cit10a_asrt(strm != nullptr);

    src_grp     grp     =   { .flag=strm->flag, .empty=true };
    if (strm->num == 0)     return  grp;

    // find file number and open dirs
    size_t          total   =   0;
    DIR     **const dirs    =   chckd_malloc(sizeof(DIR*) * strm->num, "directory DIR* scan");
    for (int i = 0; i < strm->num; ++i)     dirs[i] =   dir_scan_(strm->flag, strm->vals[i], &total);
    if (total == 0) {
        for (int i = 0; i < strm->num; ++i) if (dirs[i] != nullptr) closedir(dirs[i]);
        return  grp;
    }
    grp.empty               =   false;
    const   size_t  smap_s  =   fmap_s_(total);
    grp.files               =   new_stackmap(smap_s, sizeof(fname_itm));
    grp.open_files          =   new_stackmap(smap_s, sizeof(src_f_sm));

    // push to stackmap
    for (int i = 0; i < strm->num; ++i) {
        if (dirs[i] == nullptr)     continue;
        rewinddir(dirs[i]);

        // get extensions - update if this changes
        const   char    (*extns)[ECL];
        size_t          extns_n;
        if (strm->flag == INC_FLG) {
            extns   =   INC_EXTNS;
            extns_n =   arr_s(INC_EXTNS);
        } else if (strm->flag == SRC_FLG) {
            extns   =   SRC_EXTNS;
            extns_n =   arr_s(SRC_EXTNS);
        } else {
            cit10a_asrt(!"invalid directory scan flag");
            cit10a_exit(INTRNL_ERRNO);
        }

        struct  dirent *ent;
        while ((ent = readdir(dirs[i])) != nullptr) {
            // skip . and ..
            if (ignore_f_(ent->d_name))         continue;

            // count sources
            for (size_t j = 0; j < extns_n; ++j) {
                if (strcmp(extns[j], file_extension(ent->d_name)))      continue;

                // add file source
                const   src_slice   slc     =   fname_esc_(ent->d_name);
                const   smap_head   head    =   { .key=slc, .hash=hash_fnv1a_slc_lwr(&slc) };
                const   fname_itm   sm_itm  =   { .head=head, .folder=strm->vals[i] };

                // push file source
                smap_clsn_t         clsn_t;
                if   (c_args.case_sens)         clsn_t  =   stackmap_add(&grp.files, &sm_itm) ? smap_no_clsn_t 
                                                                                              : smap_full_clsn_t;
                else                            clsn_t  =   stackmap_add_lwr(&grp.files, &sm_itm);
                if (clsn_t == smap_no_clsn_t)   break;

                // repeated file
                const   rprt_f      err_f   =   { .file=&(src_f){ .f_name=ent->d_name }, .len=0 };
                cit10a_msg( &(msg_info){ .type=msg_warn_t, .header="repeated file", .report_f=&err_f },
                            "repeated filename in -%c%s (source ignored)", strm->flag, strm->vals[i]    );
                free((char*)slc.str);
                break;
            }
        }

        // close directory
        closedir(dirs[i]);
    }

    return  grp;
}

/*-FILE-FINDER-SINGULAR-----------------------------------------------------------------------------------------------*/

/**
 * Finds a source file from a file name. If the file is already open, will return the read file from that cache.
 * Otherwise, will attempt to open the file and cache the opened file.
 *
 * If no file with the given name could be found, will return nullptr.
 *
 * @param       f_name          file name
 * @param       grp             source file group
 * @param       err_f           error report file
 * @return                      read source file, or nullptr
 */
[[nodiscard]] static src_f *get_src_( const src_slice *const f_name,
                                            src_grp   *const grp,
                                      const rprt_f    *const err_f   ) {        // source file open
    cit10a_asrt(f_name != nullptr);
    cit10a_asrt(grp != nullptr);

    // error setup
    const   msg_info    file_err    =   { .type=msg_err_t, .header="invalid source file name", .report_f=err_f };

    // empty map
    if (grp->empty) {
        cit10a_msg(&file_err, "-%c files not provided", grp->flag);
        return  nullptr;
    }

    // look through cache
    const   src_f_sm    *const  smap_itm    =   (c_args.case_sens) ? 
                                                (src_f_sm*)stackmap_get_k_lwr(&grp->open_files, f_name) :
                                                (src_f_sm*)stackmap_get_k_lwr_lwr(&grp->open_files, f_name);
    // returned cached item
    if (smap_itm != nullptr)    return  smap_itm->source;

    // find file
    const   fname_itm   *const  source_nm   =   (c_args.case_sens) ?
                                                (fname_itm*)stackmap_get_k_lwr(&grp->files, f_name) :
                                                (fname_itm*)stackmap_get_k_lwr_lwr(&grp->files, f_name);
    if (source_nm == nullptr) {
        cit10a_msg(&file_err, "couldn't find file across -%c files", grp->flag);
        return  nullptr;
    }

    // get folder size
    const   size_t              fold_len    =   strlen(source_nm->folder);
    src_f               *const  source      =   chckd_malloc(sizeof(src_f), "source file open src_f");
    const   size_t              f_nm_o      =   (source_nm->folder[fold_len - 1] == FOLD_SEP) ? 0 : 1;

    // setup filename
    char                *const  f_nm_m      =   chckd_malloc( fold_len + f_name->len + f_nm_o + 1,
                                                              "source file name char*"             );
    memcpy(f_nm_m, source_nm->folder, fold_len);
    if (f_nm_o != 0)                            f_nm_m[fold_len]    =   FOLD_SEP;
    size_t      add_idx     =   fold_len + f_nm_o;
    for (size_t i = 0; i < f_name->len; ++i, ++add_idx) {
        // remove escape characters
        if (f_name->str[i] == ESC_CHR)  ++i;
        f_nm_m[add_idx] =   f_name->str[i];
    }
    f_nm_m[add_idx]         =   '\0';

    // read source
    *source                                 =   read_source(f_nm_m);
    free(f_nm_m);
    const   src_f_sm            smap_new    =   { .head={ .key=*f_name, .hash=hash_fnv1a_slc_lwr(f_name) },
                                                  .source=source                                            };
    [[maybe_unused]]    smap_clsn_t clsn_t;
    if   (c_args.case_sens)         clsn_t  =   stackmap_add(&grp->open_files, &smap_new) ? smap_no_clsn_t
                                                                                          : smap_full_clsn_t;
    else                            clsn_t  =   stackmap_add_lwr(&grp->open_files, &smap_new);
    cit10a_asrt(clsn_t == smap_no_clsn_t);

    return  source;
}

/*-FILE-GATHERER------------------------------------------------------------------------------------------------------*/

/**
 * Setup for multiple flag sources. The first argument should be the number of flag sources. Ensure the arguments
 * are of type strm_arr*, or this will not work.
 *
 * The cached files and file names need to be cleared. Ensure this is done with the `free_sources` function.
 *
 * @param       count           number of va args
 * @param       ...             va args
 * @return                      sources
 */
[[nodiscard]] sources get_sources(const size_t count, ...) {                    // get sources for flags
    cit10a_asrt(count != 0);

    // set up sources
    sources     srcs    =   { .num=count, .items=chckd_malloc(count * sizeof(src_grp), "sources src_grp*") };

    // unpack va args
    va_list     args;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i)      srcs.items[i]   =   file_setup_(va_arg(args, strm_arr*));
    va_end(args);
    return  srcs;
}

/*-FILE-FINDER--------------------------------------------------------------------------------------------------------*/

/**
 * Gets the source file based on a file name. The file type extension will determine which source to look through.
 *
 * @param       f_name          file name
 * @param       srcs            sources
 * @param       err_f           errpr report file
 * @return                      found source file; nullptr if no file was found
 */
[[nodiscard]] src_f *get_source( const src_slice *const f_name,
                                       sources   *const srcs,
                                 const rprt_f    *const err_f   ) {             // source file getter
    cit10a_asrt(f_name != nullptr);
    cit10a_asrt(srcs != nullptr);
    
    // get extension
    const   src_slice   extnsn  =   file_extension_slc(f_name);

    for (size_t i = 0; i < arr_s(INC_EXTNS); ++i) {
        // check include extensions
        const   src_slice   slc =   (src_slice){ .str=INC_EXTNS[i], .len=strlen(INC_EXTNS[i]) };
        if (srcslc_eq_lwr(&extnsn, &slc))   return  get_src_(f_name, &srcs->items[0], err_f);
    }

    for (size_t i = 0; i < arr_s(SRC_EXTNS); ++i) {
        // check source extensions
        const   src_slice   slc =   (src_slice){ .str=SRC_EXTNS[i], .len=strlen(SRC_EXTNS[i]) };
        if (srcslc_eq_lwr(&extnsn, &slc))   return  get_src_(f_name, &srcs->items[1], err_f);
    }

    // invalid extension
    cit10a_msg( &(msg_info){ .type=msg_err_t, .header="invalid file extension", .report_f=err_f},
                "invalid file extension for code source"                                          );
    return  nullptr;
}

/**
 * Gets a source for a static source lookup (after preprocessing).
 * The file type extension will determine which source to look through.
 *
 * @param       f_name          file name
 * @param       srcs            sources
 * @return                      found source file; should never be nullptr
 */
[[nodiscard]] src_f *get_source_static( const src_slice *const f_name,
                                        const sources   *const srcs    ) {      // static source file getter
    cit10a_asrt(f_name != nullptr);
    cit10a_asrt(srcs != nullptr);
    
    // get extension
    const   src_slice   extnsn  =   file_extension_slc(f_name);

    for (size_t i = 0; i < arr_s(INC_EXTNS); ++i) {
        const   src_grp     grp =   srcs->items[0];
        // check include extensions
        const   src_slice   slc =   (src_slice){ .str=INC_EXTNS[i], .len=strlen(INC_EXTNS[i]) };
        if (!srcslc_eq_lwr(&extnsn, &slc))  continue;
        const   src_f_sm    *const  smap_itm    =   (c_args.case_sens) ? 
                                                    (src_f_sm*)stackmap_get_k_lwr(&grp.open_files, f_name) :
                                                    (src_f_sm*)stackmap_get_k_lwr_lwr(&grp.open_files, f_name);
        cit10a_asrt(smap_itm != nullptr);
        return  smap_itm->source;
    }

    for (size_t i = 0; i < arr_s(SRC_EXTNS); ++i) {
        const   src_grp     grp =   srcs->items[1];
        // check source extensions
        const   src_slice   slc =   (src_slice){ .str=SRC_EXTNS[i], .len=strlen(SRC_EXTNS[i]) };
        if (!srcslc_eq_lwr(&extnsn, &slc))  continue;
        const   src_f_sm    *const  smap_itm    =   (c_args.case_sens) ? 
                                                    (src_f_sm*)stackmap_get_k_lwr(&grp.open_files, f_name) :
                                                    (src_f_sm*)stackmap_get_k_lwr_lwr(&grp.open_files, f_name);
        cit10a_asrt(smap_itm != nullptr);
        return  smap_itm->source;
    }

    // invalid extension
    cit10a_asrt(!"invalid file grab after preprocessing");
    cit10a_exit(INTRNL_ERRNO);
}

/*-FILE-GATHERER-CLEANER----------------------------------------------------------------------------------------------*/

/**
 * Frees allocated names from stackmap.
 *
 * @param       itm             item to free
 */
static void free_fname_itm_(fname_itm *const itm) {                             // frees file name item
    cit10a_asrt(itm != nullptr);
    free((void*)itm->head.key.str);
}

/**
 * Frees cached file item from stackmap.
 *
 * @param       itm             item to free
 */
static void free_src_f_sm_(src_f_sm *const itm) {                               // frees source file item
    cit10a_asrt(itm != nullptr);
    free_src_f(itm->source);
    free(itm->source);
}

/**
 * Frees the sources struct fully.
 *
 * @param       srcs            sources
 */
void free_sources(sources *const srcs) {                                        // frees source files
    cit10a_asrt(srcs != nullptr);
    for (size_t i = 0; i < srcs->num; ++i) {
        if (srcs->items[i].empty)       continue;

        // free names
        stackmap    f_smap  =   srcs->items[i].files;
        for (size_t j = 0; j < f_smap.buckets; ++j) {
            for (int k = 0; k < f_smap.heads[j].len; ++k)       free_fname_itm_(peek_stack(&f_smap.heads[j], k));
        }
        free_stackmap(&f_smap);

        // free files
        stackmap    c_smap  =   srcs->items[i].open_files;
        for (size_t j = 0; j < c_smap.buckets; ++j) {
            for (int k = 0; k < c_smap.heads[j].len; ++k)       free_src_f_sm_(peek_stack(&c_smap.heads[j], k));
        }
        free_stackmap(&c_smap);
    }
    free(srcs->items);
}
