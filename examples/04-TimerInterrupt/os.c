#include "os.h"

int os_main(void)
{
	lib_puts("OS start\r\n");
	timer_init(); // start timer interrupt ...
	while (1) {
		lib_puts("os loop\r\n");
		lib_delay(1000);
	} // os : do nothing, just loop!
	return 0;
}

