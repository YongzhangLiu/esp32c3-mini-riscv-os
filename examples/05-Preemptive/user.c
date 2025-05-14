#include "os.h"

void user_task0(void)
{
	lib_puts("Task0: Created!\r\n");
	while (1)
	{
		lib_puts("Task0: Running...\r\n");
		lib_delay(1000);
	}
}

void user_task1(void)
{
	lib_puts("Task1: Created!\r\n");
	while (1)
	{
		lib_puts("Task1: Running...\r\n");
		lib_delay(1000);
	}
}

void user_task2(void)
{
	lib_puts("Task2: Created!\r\n");
	while (1)
	{
		lib_puts("Task2: Running...\r\n");
		lib_delay(1000);
	}
}

void user_task3(void)
{
	lib_puts("Task3: Created!\r\n");
	while (1)
	{
		lib_puts("Task3: Running...\r\n");
		lib_delay(1000);
	}
}

void user_init()
{
	task_create(&user_task0);
	task_create(&user_task1);
	task_create(&user_task2);
	task_create(&user_task3);
}
