C_SOURCES = $(shell find source -name '*.c')
HEADERS = $(shell find include -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find source -name '*.s')
ASM_OBJ = ${ASM_SOURCES:.s=.o}

GCC_INCLUDE := -Iinclude/base -Iinclude/drivers -Iinclude/process -Iinclude/lib -Iinclude/syscall


%.o : %.c ${HEADERS}
	i686-elf-gcc $(GCC_INCLUDE) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

%.o : %.s
	i686-elf-as $< -o $@

%.o : %.asm
	nasm $< -f bin -o $(basename $<).bin
	i686-elf-as -o $@ /dev/null
	objcopy --add-section=.$(basename $<)=$(basename $<).bin $@ $@

%.o : %.data
	i686-elf-as -o $@ /dev/null
	objcopy --add-section=.$(basename $<)=$< $@ $@

all: kernel_o

# Build the kernel binary
kernel_o: kernel_entry.o main.o ap_setup.o font_bitmap.o ${C_OBJ} ${ASM_OBJ}
	i686-elf-gcc -T link.ld -o kernel.o -ffreestanding -O2 -nostdlib $^ -lgcc


# Build the kernel binary
kernel_bin: kernel_entry.o main.o ap_setup.o font_bitmap.o ${C_OBJ} ${ASM_OBJ}
	i686-elf-ld -T bios_link.ld -e _start -o kernel.bin $^ --oformat binary





clean:
	find . -name '*.o' -type f -delete
	find . -name '*.bin' -type f -delete
	find . -name '*.dis' -type f -delete

