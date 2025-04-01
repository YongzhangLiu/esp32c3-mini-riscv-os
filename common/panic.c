#include <stdint.h>
#include <stddef.h>
#include "rvruntime-frames.h"

extern int uart_tx_one_char(uint8_t c);

int panic_print_char(uint8_t c)
{
    uart_tx_one_char(c);
    return 0;
}

void panic_print_str(const char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        panic_print_char(str[i]);
    }
}

void panic_print_hex(int h)
{
    int x;
    int c;
    // Does not print '0x', only the digits (8 digits to print)
    for (x = 0; x < 8; x++) {
        c = (h >> 28) & 0xf; // extract the leftmost byte
        if (c < 10) {
            panic_print_char('0' + c);
        } else {
            panic_print_char('a' + c - 10);
        }
        h <<= 4; // move the 2nd leftmost byte to the left, to be extracted next
    }
}

void panic_print_dec_two_digits(int d)
{
    // can print at most 2 digits!
    int n1, n2;
    n1 = d % 10; // extract ones digit
    n2 = d / 10; // extract tens digit
    if (n2 == 0) {
        panic_print_char(' ');
    } else {
        panic_print_char(n2 + '0');
    }
    panic_print_char(n1 + '0');
}


static void dump_stack(RvExcFrame *frame, int exccause)
{
    uint32_t i = 0;
    uint32_t sp = frame->sp;
    panic_print_str("\n\nStack memory:\n");
    const int per_line = 8;
    for (i = 0; i < 1024; i += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + i);
        panic_print_hex(sp + i);
        panic_print_str(": ");
        for (int y = 0; y < per_line; y++) {
            panic_print_str("0x");
            panic_print_hex(spp[y]);
            panic_print_char(y == per_line - 1 ? '\n' : ' ');
        }
    }
    panic_print_str("\n");
}

static const char *desc[] = {
    "MEPC    ", "RA      ", "SP      ", "GP      ", "TP      ", "T0      ", "T1      ", "T2      ",
    "S0/FP   ", "S1      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
    "A6      ", "A7      ", "S2      ", "S3      ", "S4      ", "S5      ", "S6      ", "S7      ",
    "S8      ", "S9      ", "S10     ", "S11     ", "T3      ", "T4      ", "T5      ", "T6      ",
    "MSTATUS ", "MTVEC   ", "MCAUSE  ", "MTVAL   ", "MHARTID "
};

static const char *reason[] = {
    NULL,
    NULL,
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store address misaligned",
    "Store access fault",
};

void panic_handler(RvExcFrame *frame, int exccause)
{
#define DIM(arr) (sizeof(arr)/sizeof(*arr))

    const char *exccause_str = "Unhandled interrupt/Unknown cause";

    if (exccause < DIM(reason) && reason[exccause] != NULL) {
        exccause_str = reason[exccause];
    }

    panic_print_str("Guru Meditation Error: Core 0 panic'ed ");
    panic_print_str(exccause_str);
    panic_print_str("\n");
    panic_print_str("Core 0 register dump:\n");

    uint32_t* frame_ints = (uint32_t*) frame;
    for (int x = 0; x < DIM(desc); x++) {
        if (desc[x][0] != 0) {
            const int not_last = (x + 1) % 4;
            panic_print_str(desc[x]);
            panic_print_str(": 0x");
            panic_print_hex(frame_ints[x]);
            panic_print_char(not_last ? ' ' : '\n');
        }
    }

    dump_stack(frame, exccause);

    /* idf-monitor uses this string to mark the end of a panic dump */
    panic_print_str("ELF file SHA256: No SHA256 Embedded\n");

    while (1) {
    }
}
