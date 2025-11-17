# Project Description
This project is built on [esp32c3-direct-boot-example](https://github.com/espressif/esp32c3-direct-boot-example), attempting to implement labs of [mini-riscv-os](https://github.com/cccriscv/mini-riscv-os) on the ESP32C3 SoC.
- `riscv-none-elf-gcc` toolchain is required

## Progress
- Ported Lab 4 - Timer Interrupt, build and flash
- Ported Lab 5 - Preemptive

## Notes
- Task stacks are schieved using C style `uint32_t` arrays
