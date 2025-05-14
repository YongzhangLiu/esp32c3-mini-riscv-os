#include "os.h"
#include "task.h"
#include "lib.h"

extern void trap_init(void);
extern int task_top[MAX_TASK];
extern int task_stack_top;

void os_start()
{
	lib_puts("OS start\n");
	user_init();
	// set the current task top to the first task stack top
	// next context switch will happen in the first timer interrupt
	// we can also set ctx_swtch_req to 1 and trigger a software interrupt
	task_stack_top = task_top[0];
	timer_init();
}

void os_sched()
{
	task_switch();
}

int os_main(void)
{
	os_start();
	lib_printf("fatal error: should not reach here\r\n");
	return 0;
}
