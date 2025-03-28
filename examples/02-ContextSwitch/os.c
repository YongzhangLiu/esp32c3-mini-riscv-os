#include "os.h"

#define STACK_SIZE 1024
reg_t task0_stack[STACK_SIZE];
struct context ctx_os;
struct context ctx_task;

extern void sys_switch();

void user_task0(void)
{
	lib_puts("Task0: Context Switch to user_task0!\r\n");
	sys_switch(&ctx_task, &ctx_os);
	while (1) {} // stop here.
}

int os_main(void)
{
	lib_puts("Main: OS start\r\n");
	ctx_task.ra = (reg_t) user_task0;
	ctx_task.sp = (reg_t) &task0_stack[STACK_SIZE-1];
	sys_switch(&ctx_os, &ctx_task);
	lib_puts("Main: Context Switch to Main!\r\n");
	lib_puts("Main: OS End\r\n");
	return 0;
}

