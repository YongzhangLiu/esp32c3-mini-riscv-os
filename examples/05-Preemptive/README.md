# 05-Preemptive

## Building and running the example

1. Make sure you have a `riscv-none-elf-gcc` toolchain installed and added to PATH.
2. Build the example with CMake:
   ```bash
   cd examples/mini_os
   mkdir build
   cmake -B build -D target=esp32c3 -G Ninja .
   cmake --build build
   ```
   The following files will be generated:
   - `mini_os` — ELF output file
   - `mini_os.bin` — binary file for flashing into the chip
   - `mini_os.map` — linker map file
3. Flash the example using [esptool](https://pypi.org/project/esptool/):
   ```bash
   esptool.py --baud 921600 write_flash 0x0000 build/mini_os.bin
   ```
   (Adjust the serial port name as needed.)
4. Check Assembly
   You can easily check the machine code by:
   ```bash
   riscv-none-elf-objdump -S build/mini_os > build/mini_os.asm
   ```
   This will generate the assembly code with its source code in C.


## Preemptive Context Switching

Recall that a process contains:
1. .text(R/X) in RAM
2. .rodata(R) in RAM
3. heap & stack(R/W) in RAM
4. $pc indicating the next instruction to execute
5. $sp indicating the stack top
6. general purpose registers containing current state of process

To ensure a success context switching, we need to push registers of current task to RAM and pop registers of the next task from RAM.

in lab02, only callee-saved registers are required to be saved/restored. This is due to the riscv calling convension, which automatically generate the assembly code to save & restore caller-saved registers when we call `sys_switch()`.

Caller-saved registers are those registers that should be saved by caller, if the old register value is still needed after function call. Caller-saved registers are:
1. a0-a7 (argument registers). Most function arguments are never used after callee returns. Therefore, caller doesnot need to save them. Meanwhile, C function arguments can also be output, saving & restoring arguments will introduce extra complexity.
2. t0-t6 (temporary registers). Callee can modify the content freely as caller manages the saving and restoration before and after function call.
3. ra (return address): Caller must save its return address before jumping to callee.

Callee-saved registers are those registers that should be kept persistent across multiple function calls. If Callee changes the value of the registers, it must resume its previous value before callee returns. Callee-saved registers are:
1. sp: stack pointer
2. s0: frame pointer
3. saved registers s1-s7

Neither caller-saved nor callee-saved:
1. gp: global pointer, useful for linker to do linking relaxation
2. tp: thread pointer, used for thread-local storage (TLS)
3. x0: hardwired to 0, no need to save

in lab05, context switching happens due to timer interrupt. interrupt is asynchronous exception control flow and doesnot follow the calling convension. Therefore, to ensure control flow can be successfully switch back from interrupt context to normal task context, we need to manually save and restore all registers.

An important difference between a task-to-task context switch and an interrupt-to-task switch is, return address of function call is indicated by ra in former case, while by mepc in latter case.

This porting doesnot follow the original way of context switch by mini-rvos. To be more specific, context switching in mini-rvos happens in os context. When there is a timer interrupt, mini-rvos always switches to os context first by overwriting mepc. After interrupt exits, mini-rvos jumps to os context where an infinite loop figures out the next task to run and calls `switch_to()` to jump to the task context. 

In this porting, context switching happens directly in interrupt context. A separate interrupt context is created for executing interrupt. This is to prevent stackoverflow when executing interrupt on a nealy full task stack.

Context switching in this porting is straight-forward. It is merely a pointer switching from the stack top of the old task to the stack top of the new task. Context saving happens at the entry of interrupt handler, and context restoring happens at the exit of interrupt handler.

When creating a new task, we also need to construct the context at the stack top so that when context switching happens, the initial context can be loaded to CPU registers.

Compared to original mini-rvos:
1. This porting is more more straightforward. No need to jump back to os context before switching to a new task context.
2. This porting is more efficient. we already saved registers at the entry of interrupt routine and restore them at the exit. In mini-rvos, however, context saving & restoring happen twice.
