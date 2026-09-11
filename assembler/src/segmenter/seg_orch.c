/**
 * src/segmenter/seg_orch.c
 * Segment orchestrator.
 */

#include    <pthread.h>
#include    <stdatomic.h>

#include    "datastructures/stack.h"
#include    "datastructures/stackmap.h"
#include    "output/external.h"
#include    "output/errors.h"
#include    "argparse/argparse.h"
#include    "reader/reader.h"
#include    "preprocessor/folder_parse.h"
#include    "segmenter/constseg.h"
#include    "segmenter/dataseg.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"

/*-STORAGE-ITEMS------------------------------------------------------------------------------------------------------*/

static      atomic_bool     seg_end_flg;                                        // segmenter end assembly flag

/*-SEGMENTER-THREAD-CALLS---------------------------------------------------------------------------------------------*/

typedef struct {                                                                // constseg call args
    const   src_f       *const  source;
    const   sources     *const  srcs;
            stackmap    *const  smap;
} cseg_args;
/**
 * Constseg call (thread variant).
 *
 * @param       cseg_v          cseg_args (above)
 * @return                      nullptr
 */
static void *constseg_call(void *const cseg_v) {                                // constseg call
    cit10a_asrt(cseg_v != nullptr);

    const   cseg_args   *const  cseg        =   (cseg_args*)cseg_v;
    if (constseg(cseg->source, cseg->srcs, cseg->smap)) {
        atomic_store_explicit(&seg_end_flg, true, memory_order_relaxed);
    }
    return  nullptr;
}

typedef struct {                                                                // dataseg call args
    const   src_f       *const  source;
    const   sources     *const  srcs;
            stackmap    *const  smap;
} dseg_args;
/**
 * Dataseg call (thread variant).
 *
 * @param       dseg_v          dseg_args (above)
 * @return                      nullptr
 */
static void *dataseg_call(void *const dseg_v) {                                 // dataseg call
    cit10a_asrt(dseg_v != nullptr);

    const   dseg_args   *const  dseg        =   (dseg_args*)dseg_v;
    int                         org         =   0;
    if (dataseg(&org, dseg->source, dseg->srcs, dseg->smap)) {
        atomic_store_explicit(&seg_end_flg, true, memory_order_relaxed);
    }
    return  nullptr;
}

typedef struct {                                                                // headerseg call args
    const   src_f       *const  source;
    const   sources     *const  srcs;
            headermap   *const  hmap;
} hseg_args;
/**
 * Headerseg call (thread variant).
 *
 * @param       hseg_v          hseg_args (above)
 * @return                      nullptr
 */
static void *headerseg_call(void *const hseg_v) {                               // headerseg call
    cit10a_asrt(hseg_v != nullptr);

    const   hseg_args   *const  hseg        =   (hseg_args*)hseg_v;
    int                         org         =   0;
    if (headerseg(&org, hseg->source, hseg->srcs, hseg->hmap)) {
        atomic_store_explicit(&seg_end_flg, true, memory_order_relaxed);
    }
    return  nullptr;
}

/*-SEGMENTER-ORCHESTRATOR---------------------------------------------------------------------------------------------*/

/**
 * Segmenter orchestrator. Attempts to split across threads; otherwise, inlines calls.
 *
 * @param       source          source file
 * @param       srcs            sources
 * @return                      segmaps
 */
[[nodiscard]] segmaps segment( const src_f   *const source,
                               const sources *const srcs    ) {                 // segment orchestrator
    cit10a_asrt(source != nullptr);

    atomic_store_explicit(&seg_end_flg, false, memory_order_relaxed);

    // setup segmap
    segmaps segmap  =   (segmaps){ .constmap=new_stackmap_aln(CONST_BUCKETS, sizeof(const_var)),
                                   .datamap=new_stackmap_aln(DATA_BUCKETS, sizeof(data_var)),
                                   .headmap={ .stmts=new_stack_aln(sizeof(ln_info), LINE_INIT),
                                              .smap=new_stackmap_aln(HEADER_BUCKETS, sizeof(header_var)) } };

    // call arg setup
    hseg_args   hseg    =   { .source=source, .hmap=&segmap.headmap,  .srcs=srcs };
    dseg_args   dseg    =   { .source=source, .smap=&segmap.datamap,  .srcs=srcs };
    cseg_args   cseg    =   { .source=source, .smap=&segmap.constmap, .srcs=srcs };

    // setup thread items
    pthread_t   threads[SEGMENT_NUM - 1];
    void       *(*thrd_fns[SEGMENT_NUM - 1])(void*)     =   { headerseg_call, dataseg_call };
    void       *thrd_data[SEGMENT_NUM - 1]              =   { &hseg, &dseg };
    char       *thrd_name[SEGMENT_NUM - 1]              =   { "headerseg", "dataseg" };
    bool        live[SEGMENT_NUM - 1]                   =   { 0 };

    if (c_args.verbosity >= EXTRA_PRNT) {
        // thread output
        printf( CLR_DIM "%d new thread(s) (1 inline)\x1b[0m\n",
                ((int)SEGMENT_NUM - 1 > c_args.n_thrds - 1) ? c_args.n_thrds - 1 : (int)SEGMENT_NUM - 1 );
    }

    // thread spawn
    for (int thrd_num = 0; thrd_num < (int)SEGMENT_NUM - 1; ++thrd_num) {
        if (thrd_num + 2 > c_args.n_thrds)      break;
        if (pthread_create(&threads[thrd_num], nullptr, thrd_fns[thrd_num], thrd_data[thrd_num])) {
            cit10a_msg( &(msg_info){ .type=msg_intrnl_wrn_t, .header="thread failure" },
                        "%s thread spawn failure (non-critical)", thrd_name[thrd_num]    );
            continue;
        }
        live[thrd_num]  =   true;
    }

    // inlined cseg call
    constseg_call(&cseg);

    // thread cleanup
    for (int thrd_num = 0; thrd_num < (int)SEGMENT_NUM - 1; ++thrd_num) {
        if   (live[thrd_num])   pthread_join(threads[thrd_num], nullptr);
        else                    thrd_fns[thrd_num](thrd_data[thrd_num]);
    }

    if (atomic_load_explicit(&seg_end_flg, memory_order_relaxed))   cit10a_exit(SEGMENT_ERRNO);
    return  segmap;
}

/*-SEGMAP-PRINTER-FUNCTIONS-------------------------------------------------------------------------------------------*/

/**
 * Prints full segmap lookup tables.
 *
 * @param       segmap          segmap
 */
void print_segmap(const segmaps *const segmap) {                                // print segmap struct
    cit10a_asrt(segmap != nullptr);

    printf( DEBUG_DELIM CLR_DIM " [[ segmaps %zucnst::%zuvar::%zuhead ]]\x1b[0m\n",
            segmap->constmap.elements, segmap->datamap.elements, segmap->headmap.smap.elements );

    printf(CLR_DIM "constant map ");
    print_const_map(&segmap->constmap);
    printf(CLR_DIM "data map ");
    print_data_map(&segmap->datamap);
    printf(CLR_DIM "header map ");
    print_header_map(&segmap->headmap);

    fputs(DEBUG_DELIM "\n", stdout);
}

/**
 * Prints source file struct info.
 *
 * @param       segmap          segmap
 */
void print_segmap_info(const segmaps *const segmap) {                           // print segmap struct info
    cit10a_asrt(segmap != nullptr);
    cit10a_msg( &(msg_info){ .type=msg_vrbse_t, .header="lookup tables created" },
                             "%zu constant(s), %zu variable(s), %zu header(s)",
                             segmap->constmap.elements, segmap->datamap.elements, segmap->headmap.smap.elements );
}

/*-SEGMAP-FREE-FUNCTION-----------------------------------------------------------------------------------------------*/

/**
 * Frees the segmap fully.
 *
 * @param       segmap          segmap
 */
void free_segmap(segmaps *const segmap) {                                       // segmap free
    cit10a_asrt(segmap != nullptr);

    free_stackmap(&segmap->constmap);
    free_stackmap(&segmap->datamap);
    free_stackmap(&segmap->headmap.smap);
    free_stack(&segmap->headmap.stmts);
}
