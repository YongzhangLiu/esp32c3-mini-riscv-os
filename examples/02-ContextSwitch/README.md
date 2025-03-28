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
