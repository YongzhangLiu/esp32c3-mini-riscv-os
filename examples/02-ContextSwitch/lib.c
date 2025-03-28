#include "lib.h"

void lib_delay(volatile int count)
{
	count *= 50000;
	while (count--);
}

extern int uart_tx_one_char(uint8_t c);

int lib_putc(char ch) {
	uart_tx_one_char(ch);
    return 0;
}

void lib_puts(char *s) {
	while (*s) lib_putc(*s++);
}