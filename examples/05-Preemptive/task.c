#include "task.h"
#include "lib.h"
#include "rvruntime-frames.h"

uint32_t task_stack[MAX_TASK][STACK_SIZE] __attribute__((aligned(16))); // stack for task context
int current_task = -1; // -1 indicates the boot up stack
int task_top[MAX_TASK]; // top of each task stack
int task_stack_top = 0; // current task top
int task_num=0;  // total number of task

// create a new task
int task_create(void (*task)(void))
{
	int i=task_num++;
	long sp = (long)&(task_stack[i][STACK_SIZE - 32]); // 32 registers * 4 bytes
	RvExcFrame *frame = (RvExcFrame *)sp;
	frame->mepc = (reg_t)task; // set the return address to task function
	frame->sp = sp;
	frame->tp = i;
	task_top[i] = sp;
	lib_printf("Task created: %d\r\n", i);
	return i;
}

// switch to the next task
void task_switch(void)
{
	lib_printf("Task switched from: %d\r\n", current_task);
	if (current_task != -1) { // first time switch to the task
		task_top[current_task] = task_stack_top; // save the current task stack top
	}
	current_task = (current_task + 1) % task_num; // Round Robin Scheduling
	task_stack_top = task_top[current_task]; // load the next task stack top
	lib_printf("Task switched to: %d\r\n", current_task);
}
