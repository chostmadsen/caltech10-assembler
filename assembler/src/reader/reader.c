/**
 * src/reader/reader.c
 * File reading functions.
 */

#include    <stddef.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <errno.h>

#include    "helpers/mem.h"
#include    "helpers/general.h"
#include    "output/messages.h"
#include    "output/errors.h"
#include    "reader/reader.h"

/*-INTERNAL-READER-FUNCTIONS------------------------------------------------------------------------------------------*/

/**
 * Reads source file to memory. Caller is responsible for managing allocated memory.
 *
 * @param       f_name          file name
 * @param       f_size          read file size
 * @return                      read source file char buffer
 */
[[nodiscard]] static char *buffer_read_src_( const char   *const f_name,
                                                   size_t *const f_size  ) {    // read source to buffer
    // error report setup
    const   rprt_f      err_f   =   { .file=&(src_f){ .f_name=(char*)f_name, .len=0 } };
    const   msg_info    f_err   =   { .type=msg_err_t, .header="file error", .report_f=&err_f };

    // open file
    FILE   *const       fp      =   fopen(f_name, "rb");
    if (fp == nullptr) {
        // invalid file
        cit10a_msg(&f_err, "couldn't open / read file");
        cit10a_exit(READER_ERRNO);
    }

    // get file info
    long    ftell_s;
    if (fseek(fp, 0, SEEK_END) != 0 || (ftell_s = ftell(fp)) < 0) {
        // read error
        cit10a_msg(&f_err, "couldn't determine filesize");
        goto    read_cleanup;
    }

    // allocate file buffer
    char    *const      buf     =   chckd_malloc((size_t)ftell_s + F_PAD + 1, "text buf char*");
    memset(buf + ftell_s, '\0', F_PAD * sizeof(char));

    // read file
    rewind(fp);
    const   size_t      n_rd    =   fread(buf, 1, (size_t)ftell_s, fp);
    if (n_rd != (size_t)ftell_s) {
        // buffer copy error
        cit10a_msg(&f_err, "only read %zu / %zu bytes of source file", n_rd, (size_t)ftell_s);
        free(buf);
        goto    read_cleanup;
    }
    // report output and cleanup
    *f_size                     =   (size_t)ftell_s;
    fclose(fp);
    return  buf;

read_cleanup:
    // close and exit
    fclose(fp);
    cit10a_exit(READER_ERRNO);
}

/*-SOURCE-FILE-STRUCT-SETUP-------------------------------------------------------------------------------------------*/

/**
 * Reads source file to memory, then format it into a source file struct.
 * Freeing should be done by the `free_src_f` function.
 *
 * @param       f_name          file name
 * @return                      read source file struct
 */
[[nodiscard]] src_f read_source(const char *const f_name) {                     // read source file
    // read file
    size_t          f_size;
    char    *const  file_buf    =   buffer_read_src_(f_name, &f_size);
    cit10a_asrt(file_buf != nullptr);

    // line index array
    size_t          ln_num      =   1;
    for (size_t i = 0; file_buf[i] != '\0'; ++i) {
        if (file_buf[i] == '\n' && file_buf[i + 1] != '\0')    ++ln_num;
    }
    size_t  *const  ln_idxs     =   chckd_malloc(ln_num * sizeof(size_t), "line indexes size_t*");
    ln_idxs[0]                  =   0;

    // track lines
    size_t          idx_num     =   1;
    for (size_t i = 0; i < f_size; ++i) {
        // skip over non-newlines
        if (file_buf[i] == '\r')                            file_buf[i] = '\0';
        if (file_buf[i] != '\n')                            continue;
        // replace newline
        file_buf[i]             =   '\0';

        // track line indexes
        if (i + 1 >= f_size || file_buf[i + 1] == '\0')     continue;
        // add new line index
        ln_idxs[idx_num++]      =   i + 1;
    }
    cit10a_asrt(idx_num <= ln_num);

    // strcpy file name and return
    char    *const  file_name   =   chckd_malloc(strlen(f_name) + 1, "file_name char*");
    strcpy(file_name, f_name);

    // struct return
    return  (src_f){ .f_name=file_name, .text=file_buf,          .ln_idxs=ln_idxs, .ln_num=idx_num,
                     .len=f_size,       .size=f_size + F_PAD + 1                                    };
}

/*-SOURCE-FILE-STRUCT-GETLINE-----------------------------------------------------------------------------------------*/

/**
 * Gets a line at the given index from the given source file struct.
 *
 * @param       source_f        source file struct
 * @param       ln              line to read
 */
[[nodiscard]] const char *src_f_getline( const src_f  *const source_f,
                                         const size_t        ln        ) {      // readline of struct
    cit10a_asrt(source_f != nullptr);
    cit10a_asrt(source_f->text != nullptr && source_f->ln_idxs != nullptr);
    cit10a_asrt(ln < source_f->ln_num);

    return  &source_f->text[source_f->ln_idxs[ln]];
}

/*-SOURCE-FILE-STRUCT-CLEANUP-----------------------------------------------------------------------------------------*/

/**
 * Frees source file struct internals.
 *
 * @param       source_f        source file struct
 */
void free_src_f(src_f *source_f) {                                              // free source struct
    cit10a_asrt(source_f != nullptr);
    free(source_f->f_name);
    safe_free(source_f->ln_idxs);
    safe_free(source_f->text);

#ifndef NDEBUG
    // struct zero
    source_f->f_name    =   nullptr;
    source_f->ln_num    =   0;
    source_f->size      =   0;
    source_f->len       =   0;
#endif  /* NDEBUG */
}

/*-SOURCE-FILE-STRINGPOINTER-NEW-LINE---------------------------------------------------------------------------------*/

/**
 * Adjusts a string pointer one line forward from the source file.
 *
 * @param       sptr            string pointer
 * @param       source          source file
 * @return                      whether an error occurred
 */
[[nodiscard]] bool newln_strptr(       strptr *const sptr, 
                                 const src_f  *const source ) {                 // strptr newline
    // verify overflow
    if (sptr->ln + 1 >= source->ln_num)     return  true;

    // get new line
    ++sptr->ln;
    sptr->col   =   0;
    sptr->str   =   src_f_getline(source, sptr->ln);
    return  false;
}

/**
 * Sets a string pointer to the specified line (must be in-bounds).
 *
 * @param       sptr            string pointer
 * @param       source          source file
 * @param       ln              line
 * @return                      whether an error occurred
 */
void setln_strptr(       strptr *const sptr,
                   const src_f  *const source, 
                   const size_t        ln      ) {                              // strptr set line
    cit10a_asrt(ln < source->ln_num);

    // get new line
    sptr->ln    =   ln;
    sptr->col   =   0;
    sptr->str   =   src_f_getline(source, ln);
}
