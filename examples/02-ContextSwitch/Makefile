CC = riscv64-unknown-elf-gcc
# zicsr is required to build csrr
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -march=rv32imc_zicsr_zifencei -mabi=ilp32

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 4 -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)
# QEMU's gdb stub command line changed in 0.11
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

all: os.elf

os.elf: start.s sys.s lib.c os.c
	$(CC) $(CFLAGS) -T os.ld -o os.elf $^ -Wl,-Map=output.map

qemu: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel os.elf

qemu-gdb: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-C to exit QEMU. Now run 'gdb' in another window."
	$(QEMU) $(QFLAGS) -kernel os.elf -S $(QEMUGDB)
clean:
	rm -f *.elf
