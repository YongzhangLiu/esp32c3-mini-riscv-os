#include "stdint.h"

#define ISR_STACK_SIZE 1024

uint32_t isr_stack[ISR_STACK_SIZE] __attribute__((aligned(16))); // stack for interrupt context
uint32_t *isr_stack_top = &isr_stack[ISR_STACK_SIZE]; // top of the isr stack, 16-byte aligned

int ctx_swtch_req = 0; // 0: no context switch, 1: context switch

void _global_interrupt_handler(int mcause)
{
    ctx_swtch_req = 0; // context switch is not requested for normal external interrupt

    if (mcause == 0x80000001) {
        extern void timer_handler(void);
        timer_handler();
        ctx_swtch_req = 1; // context switch is requested for timer interrupt
    }
    else {
        printf("unknown interrupt: %d\r\n", mcause);
    }
}
