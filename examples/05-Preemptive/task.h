#ifndef __TASK_H__
#define __TASK_H__

#include "riscv.h"

#define MAX_TASK 4
#define STACK_SIZE 256

int task_create(void (*task)(void));
void task_switch(void);

#endif
