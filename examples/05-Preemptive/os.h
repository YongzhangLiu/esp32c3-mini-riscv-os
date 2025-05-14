#ifndef __OS_H__
#define __OS_H__

#include "riscv.h"
#include "lib.h"
#include "task.h"
#include "timer.h"

extern void user_init();
int  os_main(void);
void os_sched(void);
void os_start(void);
#endif
