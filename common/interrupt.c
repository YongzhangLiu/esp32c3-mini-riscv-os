#include "stdint.h"

void _global_interrupt_handler(int mcause)
{
    if (mcause == 0x80000001) {
        timer_handler();
    }
    else {
        lib_printf("unknown interrupt: %d\r\n", mcause);
    }
}
