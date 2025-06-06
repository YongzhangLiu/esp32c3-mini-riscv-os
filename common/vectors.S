#include "rvruntime-frames.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
/* Macro which first allocates space on the stack to save general
 * purpose registers, and then save them. GP register is excluded.
 * The default size allocated on the stack is CONTEXT_SIZE, but it
 * can be overridden. */
.macro save_general_regs cxt_size=CONTEXT_SIZE
    addi sp, sp, -\cxt_size
    sw   ra, RV_STK_RA(sp)
    sw   tp, RV_STK_TP(sp)
    sw   t0, RV_STK_T0(sp)
    sw   t1, RV_STK_T1(sp)
    sw   t2, RV_STK_T2(sp)
    sw   s0, RV_STK_S0(sp)
    sw   s1, RV_STK_S1(sp)
    sw   a0, RV_STK_A0(sp)
    sw   a1, RV_STK_A1(sp)
    sw   a2, RV_STK_A2(sp)
    sw   a3, RV_STK_A3(sp)
    sw   a4, RV_STK_A4(sp)
    sw   a5, RV_STK_A5(sp)
    sw   a6, RV_STK_A6(sp)
    sw   a7, RV_STK_A7(sp)
    sw   s2, RV_STK_S2(sp)
    sw   s3, RV_STK_S3(sp)
    sw   s4, RV_STK_S4(sp)
    sw   s5, RV_STK_S5(sp)
    sw   s6, RV_STK_S6(sp)
    sw   s7, RV_STK_S7(sp)
    sw   s8, RV_STK_S8(sp)
    sw   s9, RV_STK_S9(sp)
    sw   s10, RV_STK_S10(sp)
    sw   s11, RV_STK_S11(sp)
    sw   t3, RV_STK_T3(sp)
    sw   t4, RV_STK_T4(sp)
    sw   t5, RV_STK_T5(sp)
    sw   t6, RV_STK_T6(sp)
.endm

.macro save_mepc
    csrr t0, mepc
    sw   t0, RV_STK_MEPC(sp)
.endm

/* Restore the general purpose registers (excluding gp) from the context on
 * the stack. The context is then deallocated. The default size is CONTEXT_SIZE
 * but it can be overridden. */
.macro restore_general_regs cxt_size=CONTEXT_SIZE
    lw   ra, RV_STK_RA(sp)
    lw   tp, RV_STK_TP(sp)
    lw   t0, RV_STK_T0(sp)
    lw   t1, RV_STK_T1(sp)
    lw   t2, RV_STK_T2(sp)
    lw   s0, RV_STK_S0(sp)
    lw   s1, RV_STK_S1(sp)
    lw   a0, RV_STK_A0(sp)
    lw   a1, RV_STK_A1(sp)
    lw   a2, RV_STK_A2(sp)
    lw   a3, RV_STK_A3(sp)
    lw   a4, RV_STK_A4(sp)
    lw   a5, RV_STK_A5(sp)
    lw   a6, RV_STK_A6(sp)
    lw   a7, RV_STK_A7(sp)
    lw   s2, RV_STK_S2(sp)
    lw   s3, RV_STK_S3(sp)
    lw   s4, RV_STK_S4(sp)
    lw   s5, RV_STK_S5(sp)
    lw   s6, RV_STK_S6(sp)
    lw   s7, RV_STK_S7(sp)
    lw   s8, RV_STK_S8(sp)
    lw   s9, RV_STK_S9(sp)
    lw   s10, RV_STK_S10(sp)
    lw   s11, RV_STK_S11(sp)
    lw   t3, RV_STK_T3(sp)
    lw   t4, RV_STK_T4(sp)
    lw   t5, RV_STK_T5(sp)
    lw   t6, RV_STK_T6(sp)
    addi sp,sp, \cxt_size
.endm

.macro restore_mepc
    lw      t0, RV_STK_MEPC(sp)
    csrw    mepc, t0
.endm


    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte intructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     */
    .balign 0x100
    .global _vector_table
    .type _vector_table, @function
_vector_table:
    .option push
    .option norvc
    j _panic_handler            /* exception handler, entry 0 */
    .rept 31
    j _interrupt_handler        /* 31 identical entries, all pointing to the interrupt handler */
    .endr
    .option pop
    .size _vector_table, .-_vector_table

    /* This should be the exception handler.
     * It doesn't do anything useful yet, but you can set a breakpoint here, to see if an exception happens.
     */
    .global _panic_handler
    .type _panic_handler, @function
_panic_handler:
    save_general_regs RV_STK_FRMSZ
    save_mepc

    addi  t0, sp, RV_STK_FRMSZ /* restore sp with the value when trap happened */

     /* Save CSRs */
    sw    t0, RV_STK_SP(sp)
    csrr  t0, mstatus
    sw    t0, RV_STK_MSTATUS(sp)
    csrr  t0, mcause
    sw    t0, RV_STK_MCAUSE(sp)
    csrr  t0, mtvec
    sw    t0, RV_STK_MTVEC(sp)
    csrr  t0, mhartid
    sw    t0, RV_STK_MHARTID(sp)
    csrr  t0, mtval
    sw    t0, RV_STK_MTVAL(sp)

    csrr	a1, mcause				/* exception cause */

    mv	a0, sp					    /* RvExcFrame *regs */
    call panic_handler
    .size  _panic_handler, .-_panic_handler

    /* This is the interrupt handler.
     * It is declared as a weak function, and can be overridden.
     */
    .global _interrupt_handler
    .weak _interrupt_handler
    .type _interrupt_handler, @function
_interrupt_handler:
	/* save registers & mepc to stack */
    save_general_regs
    save_mepc

    /* save current task top */
    la t0, task_stack_top
    sw sp, 0(t0)

    /* switch to isr stack */
    la t0, isr_stack_top
    lw sp, 0(t0)

    /* call _global_interrupt_handler */
    csrr a0, mcause
    call _global_interrupt_handler

    /* check if context switch is requested */
    la t0, ctx_swtch_req
    lw t0, 0(t0)
    beq t0, zero, skip_sched

    /* context switch */
    call os_sched

    /* switch to task stack */
    la t0, task_stack_top
    lw sp, 0(t0)

skip_sched:
    /* restore registers & mepc from stack */
    restore_mepc
    restore_general_regs
    /* exit, this will also re-enable the interrupts */
    mret
    .size  _interrupt_handler, .-_interrupt_handler
