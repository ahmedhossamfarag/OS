nasm boot/boot.asm -f bin -o boot.bin
gcc -m32 -c -fno-pie kernel/kernel.c -o kernel.o
ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary
cat boot.bin kernel.bin > os-image
qemu-system-x86_64 os-image

objdump -d kernel.o
ndisasm -b 32 kernel.bin > kernel.dis

dd if=inputfile of=outputfile bs=512 seek=10 conv=notrunc