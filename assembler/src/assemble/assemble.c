/**
 * src/assemble/assemble.c
 * Final assembler call.
 */

#ifndef NTHREAD
#include    <pthread.h>
#include    <stdatomic.h>
#endif  /* NTHREAD */

#include    "helpers/mem.h"
#include    "datastructures/stack.h"
#include    "output/external.h"
#include    "output/errors.h"

#ifndef NTHREAD
#include    "argparse/argparse.h"
#endif  /* NTHREAD */

#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"
#include    "assemble/assemble.h"

#ifndef NTHREAD

/*-STORAGE-ITEMS------------------------------------------------------------------------------------------------------*/

static      atomic_bool     asm_end_flg;                                        // assembler end assembly flag

/*-ASSEMBLER-THREAD-FUNCTION------------------------------------------------------------------------------------------*/

typedef struct {                                                                // asm_thread call args
    const   segmaps    *segmap;
            ln_asm     *target;
            int         start;
            int         end;
} asm_thrd_args;
/**
 * Assembly line assembler thread.
 *
 * @param       asm_thrd_args_v thread arguments (above)
 * @return                      nullptr
 */
static void *asm_thread_(void *const asm_thrd_args_v) {                         // assembler line thread
    cit10a_asrt(asm_thrd_args_v != nullptr);

    // line assemble on range
    bool                            err     =   false;
    const   asm_thrd_args   *const  args    =   (asm_thrd_args*)asm_thrd_args_v;
    for (int i = args->start; i < args->end; ++i) {
        const   ln_asm  ln_a    =   line_assemble(args->segmap, (ln_info*)peek_stack(&args->segmap->headmap.stmts, i));
        if (ln_a.la.ln == -1) {
            // set error
            err     =   true;
            continue;
        }
        args->target[i]         =   ln_a;
    }

    // set error flag
    if (err && !atomic_load_explicit(&asm_end_flg, memory_order_relaxed)) {
        atomic_store_explicit(&asm_end_flg, true, memory_order_relaxed);
    }
    return  nullptr;
}

/*-ASSEMBLER-ORCHESTRATOR---------------------------------------------------------------------------------------------*/

/**
 * Assembles a file to binary, given the segmaps, which should include all the required lines.
 * Splits the lines across multiple threads; inlines otherwise.
 *
 * @param       segmap          segmaps
 * @return                      assembly binary instructions
 */
[[nodiscard]] asm_ret assemble(const segmaps *const segmap) {                   // assembler assemble
    cit10a_asrt(segmap != nullptr);

    atomic_store_explicit(&asm_end_flg, false, memory_order_relaxed);
    // asm_ret setup
    asm_ret         ret     =   { .num_segs=segmap->headmap.stmts.len };
    if (ret.num_segs == 0) {
        ret.ln_asms         =   nullptr;
        return  ret;
    }
    ret.ln_asms             =   chckd_aln_alloc(ret.num_segs * sizeof(ln_asm), "assemble ln_asm*");

    // calculate required threads
    int             n_segs  =   ret.num_segs / (int)ASMS_PER_LN;
    const   int     n_thrds =   (n_segs >= c_args.n_thrds) ? c_args.n_thrds : n_segs;
    int             n_lns   =   ret.num_segs / n_thrds;
    n_lns                   =   (n_lns + (int)ASMS_PER_LN - 1) & ~((int)ASMS_PER_LN - 1);
    if (n_lns == 0)             n_lns = (int)ASMS_PER_LN;

    // inline call
    if (n_thrds <= 1) {
        // singular call
        asm_thread_(&(asm_thrd_args){ .segmap=segmap, .target=ret.ln_asms, .start=0, .end=ret.num_segs });
        // explicit end checka
        if (atomic_load_explicit(&asm_end_flg, memory_order_relaxed))   cit10a_exit(ASSEMBLE_ERRNO);
        return  ret;
    }

    // init thread items
    bool            live[MAX_THREADS - 1]       =   { 0 };
    asm_thrd_args   args[MAX_THREADS];
    pthread_t       threads[MAX_THREADS - 1];

    // setup thread items
    int             n_thrds_spwn    =   0;
    int             seg             =   0;
    for (int i = 0; i < n_thrds; ++i) {
        // arg setup
        args[i].segmap      =   segmap;
        args[i].target      =   ret.ln_asms;
        args[i].start       =   seg;
        // offset calculation
        seg                 +=  n_lns;
        ++n_thrds_spwn;
        if (seg >= ret.num_segs) {
            // prevent additional threads
            args[i].end     =   ret.num_segs;
            break;
        }
        args[i].end         =   (i == n_thrds - 1) ? ret.num_segs : seg;
    }

    if (c_args.verbosity >= EXTRA_PRNT) {
        // thread output
        printf( CLR_DIM "%d new thread(s) (1 inline) | %d lines / thread | %d inline adjusted\x1b[0m\n",
                n_thrds_spwn - 1, n_lns, ret.num_segs - n_thrds_spwn * n_lns                             );
    }

    // thread call
    for (int thrd = 0; thrd < n_thrds_spwn - 1; ++thrd) {
        if (pthread_create(&threads[thrd], nullptr, asm_thread_, &args[thrd])) {
            cit10a_msg( &(msg_info){ .type=msg_intrnl_wrn_t, .header="thread failure" },
                        "%d assembler thread spawn failure (non-critical)", thrd         );
            continue;
        }
        live[thrd]      =   true;
    }
    asm_thread_(&args[n_thrds_spwn - 1]);

    // thread cleanup
    for (int thrd = 0; thrd < n_thrds_spwn - 1; ++thrd) {
        if   (live[thrd])       pthread_join(threads[thrd], nullptr);
        else                    asm_thread_(&args[thrd]);
    }

    if (atomic_load_explicit(&asm_end_flg, memory_order_relaxed))   cit10a_exit(ASSEMBLE_ERRNO);
    return  ret;
}

#else

/*-ASSEMBLER-ORCHESTRATOR-(NON-THREADED)------------------------------------------------------------------------------*/

/**
 * Assembles a file to binary, given the segmaps, which should include all the required lines. non-threaded variant.
 *
 * @param       segmap          segmaps
 * @return                      assembly binary instructions
 */
[[nodiscard]] asm_ret assemble(const segmaps *const segmap) {                   // assembler assemble (non-threaded)
    cit10a_asrt(segmap != nullptr);

    // asm_ret setup
    asm_ret         ret     =   { .num_segs=segmap->headmap.stmts.len };
    if (ret.num_segs == 0) {
        ret.ln_asms         =   nullptr;
        return  ret;
    }
    ret.ln_asms             =   chckd_malloc(ret.num_segs * sizeof(ln_asm), "assemble ln_asm*");

    // line assemble on everything
    bool            err     =   false;
    for (int i = 0; i < ret.num_segs; ++i) {
        const   ln_asm  ln_a    =   line_assemble(segmap, (ln_info*)peek_stack(&segmap->headmap.stmts, i));
        if (ln_a.la.ln == -1) {
            // set error
            err     =   true;
            continue;
        }
        ret.ln_asms[i]          =   ln_a;
    }

    // error flag check
    if (err)        cit10a_exit(ASSEMBLE_ERRNO);
    return  ret;
}

#endif  /* NTHREAD */

/*-ASSEMBLER-PRINTER-FUNCTIONS----------------------------------------------------------------------------------------*/

/**
 * Prints full assembler instructions.
 *
 * @param       asm_r           assembly return
 */
void print_asm(const asm_ret *const asm_r) {                                    // print asm_ret struct
    cit10a_asrt(asm_r != nullptr);

    printf(DEBUG_DELIM CLR_DIM " [[ binary 0x%xwr ]]\x1b[0m\n", asm_r->num_segs);

    for (int i = 0; i < asm_r->num_segs; ++i) {
        // emit instructions
        printf("%04x  %04x", asm_r->ln_asms[i].la.loc, asm_r->ln_asms[i].la.instr);
        printf(CLR_DIM "  [[ %s::%d ]]\x1b[0m\n", asm_r->ln_asms[i].la.source->f_name, asm_r->ln_asms[i].la.ln + 1);
    }

    fputs(DEBUG_DELIM "\n", stdout);
}

/**
 * Prints asm_ret info.
 *
 * @param       asm_r           assembly return
 */
void print_asm_info(const asm_ret *const asm_r) {                               // print asm_ret struct info
    cit10a_asrt(asm_r != nullptr);
    cit10a_msg(&(msg_info){ .type=msg_vrbse_t, .header="assembly assembled" }, "%d instruction(s)", asm_r->num_segs);
}

/*-ASM_RET-FREE-FUNCTION----------------------------------------------------------------------------------------------*/

/**
 * Frees the asm_ret struct fully.
 *
 * @param       asm_r           assembly return
 */
void free_asm_ret(asm_ret *const asm_r) {                                       // asm_ret free
    cit10a_asrt(asm_r != nullptr);

    free(asm_r->ln_asms);
#ifndef NDEBUG
    asm_r->ln_asms  =   0;
#endif  /* NDEBUG */
}
