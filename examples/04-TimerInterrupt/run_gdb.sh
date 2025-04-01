# in one terminal
openocd -c "set ESP_RTOS none" -f "board/esp32c3-builtin.cfg"

# in another terminal
riscv32-esp-elf-gdb -x gdbinit build/mini_os
