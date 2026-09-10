/**
 * src/assemble/assemble.c
 * Final assembler call.
 */

#include    <pthread.h>
#include    <stdatomic.h>

#include    "helpers/mem.h"
#include    "datastructures/stack.h"
#include    "output/errors.h"
#include    "argparse/argparse.h"
#include    "segmenter/headerseg.h"
#include    "segmenter/seg_orch.h"
#include    "assemble/line.h"
#include    "assemble/assemble.h"

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
        if (ln_a.ln == -1) {
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
    const   int     n_segs  =   ret.num_segs / (int)ASMS_PER_LN;
    const   int     n_thrds =   (n_segs >= c_args.n_thrds) ? c_args.n_thrds : n_segs;
    const   int     n_lns   =   ret.num_segs / n_thrds;
    const   int     slack   =   ret.num_segs % n_thrds;

    // inline call
    if (n_thrds <= 1) {
        // singular call
        asm_thread_(&(asm_thrd_args){ .segmap=segmap, .target=ret.ln_asms, .start=0, .end=ret.num_segs });
        return  ret;
    }

    // init thread items
    pthread_t       threads[n_thrds - 1];
    bool            live[n_thrds - 1];
    asm_thrd_args   args[n_thrds];

    // setup thread items
    for (int i = 0; i < n_thrds - 1; ++i)       live[i] =   false;
    for (int seg = 0, i = 0; i < n_thrds; ++i) {
        // arg setup
        args[i].segmap      =   segmap;
        args[i].target      =   ret.ln_asms;
        args[i].start       =   seg;
        // offset calculation
        args[i].end         =   (seg += n_lns);
    }
    args[n_thrds - 1].end   +=  slack;

    cit10a_asrt(args[n_thrds - 1].end == ret.num_segs);

    // thread call
    for (int thrd = 0; thrd < n_thrds - 1; ++thrd) {
        if (pthread_create(&threads[thrd], nullptr, asm_thread_, &args[thrd])) {
            cit10a_msg( &(msg_info){ .type=msg_intrnl_wrn_t, .header="thread failure" },
                        "%d assembler thread spawn failure (non-critical)", thrd         );
            continue;
        }
        live[thrd]      =   true;
    }
    asm_thread_(&args[n_thrds - 1]);

    // thread cleanup
    for (int thrd = 0; thrd < n_thrds - 1; ++thrd) {
        if   (live[thrd])       pthread_join(threads[thrd], nullptr);
        else                    asm_thread_(&args[thrd]);
    }

    if (atomic_load_explicit(&asm_end_flg, memory_order_relaxed))   cit10a_exit(ASSEMBLE_ERRNO);
    return  ret;
}

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
        printf("%04x  %04x  ", asm_r->ln_asms[i].loc, asm_r->ln_asms[i].instr);
        printf(CLR_DIM "  [[ %s::%d ]]\x1b[0m\n", asm_r->ln_asms[i].source->f_name, asm_r->ln_asms[i].ln);
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
    cit10a_msg(&(msg_info){ .type=msg_vrbse_t, .header="assembly assembled" }, "%zu instruction(s)", asm_r->num_segs);
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
