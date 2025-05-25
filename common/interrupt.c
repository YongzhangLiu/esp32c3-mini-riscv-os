#include "stdint.h"
extern void timer_handler(void);
extern int  lib_printf(const char* s, ...);
void _global_interrupt_handler(int mcause)
{
    if (mcause == 0x80000001) {
        timer_handler();
    }
    else {
        lib_printf("unknown interrupt: %d\r\n", mcause);
    }
}
